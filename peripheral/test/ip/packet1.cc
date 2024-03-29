#include <iostream>
#include "tcp_server.h"
#include "hp/common/buffer/circular_buffer.h"
#include "hp/common/buffer/buffer_template.h"
#include "hp/common/buffer/fast_buffer.h"
#include "abstract_ip.h"


//The wire format of a simple packet
//                        0            2            6                  x
//                        +------------+------------+------------------+
// Name:                  |   Header   |    CMD     |       Data       |
//                        +------------+------------+------------------+
// Bytes:                 |   2 Bytes  |  2 Bytes   |    Len value     |
//                        +------------+------------+------------------+
// Packet Detail:         | 0xaa 0xff  | SHIT, FUCK |  Serialize Data  |
//                        +------------+------------+------------------+


#define SERVER_PORT (8585)
#define SERVER_IP   "127.0.0.1"

using namespace hp::peripheral;

enum MyMessages {
    FUCK = 0x01,
    SHIT = 0x02,
    Wrong = 0x03
};



class WrongMessage : public AbstractSerializableMessage {

private:
    uint32_t msg_size_;

public:
    WrongMessage() { msg_size_ = 0; }
    void serialize(char *buffer, size_t size) {
    }
    void deserialize(const char *buffer, size_t size) {
    }
    size_t get_serialize_size() const { return msg_size_; }
    int get_type() const { return MyMessages::Wrong; }
};


class FuckMessage : public AbstractSerializableMessage {

private:
    struct DateTime{
        uint32_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t min;
        uint8_t sec;
    };
    DateTime date_time_;

    uint32_t msg_size_;

public:
    FuckMessage() { msg_size_ = 9; }
    void serialize(char *buffer, size_t size) {
        memcpy(buffer, (void*) &date_time_, size);
    }
    void deserialize(const char *buffer, size_t size) {
        memcpy((void*) & date_time_, buffer, size);
    }
    size_t get_serialize_size() const { return msg_size_; }
    int get_type() const { return MyMessages::FUCK; }
    DateTime get_date_time() const { return date_time_;}
};

class ShitMessage : public AbstractSerializableMessage {
private:
    struct Student{
        int id;
        uint8_t age;
    }student_;
public:
    void serialize(char *buffer, size_t size) {
        memcpy(buffer, (void*) &student_, size);
    }
    void deserialize(const char *buffer, size_t size) {
        memcpy((void*) & student_, buffer, size);
    }
    size_t get_serialize_size() const { return 5; }
    int get_type() const { return MyMessages::SHIT; }
    Student get_student() const { return student_; }
};

class MessageFactory : public AbstractMessageFactory{
public:
    std::shared_ptr<AbstractSerializableMessage> build_message(const std::string cmd) {
        if (cmd[0] == (char)0xd1 && cmd[1] == char(0xd2))
            return std::make_shared<FuckMessage>();
        else if (cmd[0] == (char)0xd2 && cmd[1] == char(0xd3))
            return std::make_shared<ShitMessage>();
        else
            return std::make_shared<WrongMessage>();
        return nullptr;
    }
};

class ChecksumChecker : public AbstractCRC
{
public:
    bool is_valid(const char *data, size_t data_size, const char *crc_data, size_t crc_size) const {
        (void)crc_size;
        short crc_val = 0;
        for (uint32_t i = 0 ; i < data_size; i++)
            crc_val += data[i];
        short crc_data_val = *((short *)(crc_data));
        if (crc_data_val != crc_val)
            return false;
        else
            return true;
    }
};

#define HEADER1         (0xaa)
#define HEADER2         (0xff)
#define CMD_SIZE        (2)
#define LENGTH_SIZE     (2)
#define CRC_SIZE        (2)
#define FOOTER1         (0xbb)
#define FOOTER2         (0xcc)

class ClientPacket : public AbstractPacketSections {
private:
    std::vector<Section*> sections_;

public:
    ClientPacket(){
        HeaderSection* header = new HeaderSection();
        header->content = std::string{static_cast<char>HEADER1, static_cast<char>HEADER2};

        CMDSection* cmd = new CMDSection();
        cmd->size_bytes = CMD_SIZE;
        cmd->msg_factory = std::make_shared<MessageFactory>();

        DataSection* data = new DataSection();

        sections_.push_back(header);
        sections_.push_back(cmd);
        sections_.push_back(data);
    }

    std::vector<Section*> get_packet_sections(){
        return sections_;
    };

    void get_error_packet(PacketErrors error, const std::map<PacketSections, std::string>& packet){
        switch (error) {
        case PacketErrors::Wrong_CRC : {
            std::cout << "Wrong CRC" << std::endl;
            break;
        }
        case PacketErrors::Wrong_Footer : {
            std::cout << "Wrong Footer" << std::endl;
            break;
        }
        case PacketErrors::Wrong_CMD : {
            std::cout << "Wrong CMD" << std::endl;
            break;
        }
        default: {
            std::cout << "There is a new error" << std::endl;
            break;
        }
        }
    }
};

std::shared_ptr<std::thread> Client_Thread;
std::shared_ptr<boost::thread_group> Thread_Clients_;

void thread_for_work_client(TCPClientShared client) {
    int counter = 0;
    while (1) {
        auto msg = client->get_next_packet();
        if (msg->get_type() == MyMessages::FUCK) {
            auto fuck_msg = std::dynamic_pointer_cast<FuckMessage>(msg);
            std::cout << "#" << counter << "FUCK_MSG" << std::endl;
        } else if (msg->get_type() == MyMessages::SHIT) {
            auto shit_msg = std::dynamic_pointer_cast<ShitMessage>(msg);
            std::cout << "#" << counter << " SHIT_MSG" << std::endl;
        } else if (msg->get_type() == MyMessages::Wrong) {
            auto wron_msg = std::dynamic_pointer_cast<WrongMessage>(msg);
            std::cout << "#" << counter << " WRONG_MSG" << std::endl;
        }
        counter++;
    }
}

void new_connection(TCPClientShared client)
{
    auto packet = std::make_shared<ClientPacket>();
    client->extract_messages(packet.get());
    client->set_buffer_size((uint64_t)2 * 1024);
    Client_Thread = std::make_shared<std::thread>(thread_for_work_client, client);
    std::cout << "id: " << client->get_client_id() << " port: " << client->get_port() << " ip: " << client->get_ip() << std::endl;
}

void start_tcp_server()
{
    auto tcp_server = std::make_shared<TCPServer>(SERVER_PORT);
    tcp_server->notify_me_for_new_connection(std::bind(new_connection, std::placeholders::_1));
    tcp_server->start();
    std::cout << "Start tcp server with port 8585" << std::endl;
    while(1)
        std::this_thread::sleep_for(std::chrono::seconds(10));
}

void send_data_to_server() {
    TCPClient *client = new TCPClient(SERVER_IP, SERVER_PORT);
    bool con = false;

    while (!con) {
        con = client->connect();
        if (!con)
            std::cout << "Can't connect to tcp server" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::vector<std::string> all_data;
    std::string fuck_message_ok  {(char)HEADER1, (char)HEADER2, (char)0xd1,    (char)0xd2,
                (char)0x15,    (char)0x00,    (char)0x00,    (char)0x00, (char)0x05, (char)0x06, (char)0x07, (char)0x08, (char)0x09 };

    std::string shit_message_ok  {(char)HEADER1, (char)HEADER2, (char)0xd2,    (char)0xd3,
                (char)0x04,    (char)0x00,    (char)0x00,    (char)0x00, (char)0x20 };

    std::string shit_message_cmd  {(char)HEADER1, (char)HEADER2, (char)0xd2,    (char)0xd5,
                (char)0x04,    (char)0x00,    (char)0x00,    (char)0x00, (char)0x20 };

    all_data.push_back(shit_message_ok);
    all_data.push_back(fuck_message_ok);
    all_data.push_back(shit_message_cmd);

    int counter = 0;
    while(counter < 5) {
        for (auto cur_packet : all_data) {
            uint ret = client->send(cur_packet.data(), cur_packet.length());
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if (ret != cur_packet.size()) {
                std::cout << "fuuck" << std::endl;
                exit(1);
            }
        }
        counter++;
        //        std::cout << "send: " << counter++ << std::endl;
    }
}

int main()
{
    Thread_Clients_ = std::make_shared<boost::thread_group>();
    Thread_Clients_->create_thread(start_tcp_server);
    Thread_Clients_->create_thread(send_data_to_server);
    Thread_Clients_->join_all();
    Client_Thread->join();
}
