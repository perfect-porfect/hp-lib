#include <iostream>
#include "tcp_server.h"
#include "circular_buffer.h"
#include "buffer_template.h"
#include <abstract_peripheral.h>

//std::string data{(char)0xaa, (char)0xff, (char)0xd1, (char)0xd2, (char)0x00, (char)0x00,
//            (char)0x00, (char)0x09, (char)0x01, (char)0x02, (char)0x03, (char)0x04,
//            (char)0x05, (char)0x06, (char)0x07, (char)0x08, (char)0x09, (char)0xaa,
//            (char)0xbb, (char)0xcc, (char)0xdd};

//This Sample packet include:
//                              +------------+------------+------------------+------------------+--------------+
// Name:                        |   Header   |    CMD     |       Len        |       Data       |     CRC      |
//                              +------------+------------+------------------+------------------+--------------+
// Bytes:                       |   2 Bytes  |  2 Bytes   |     4 Bytes      |    Len value     |    4 Bytes   |
//                              +------------+------------+------------------+------------------+--------------+
// Packet Detail:               | 0xaa 0xff  | SHIT, FUCK | First byte's msb |  Serialize Data  | Sum of Bytes |
//                              +------------+------------+------------------+------------------+--------------+


#define SERVER_PORT (8585)
#define SERVER_IP   "127.0.0.1"

using namespace hp::peripheral;

enum MyMessages {
    FUCK = 0x01,
    SHIT = 0x02
};

class FuckMessage : public AbstractSerializableMessage {
public:
    FuckMessage() { msg_size_ = sizeof (date_time_);}
    void serialize(char *buffer, size_t size) {
        memcpy(buffer, (void*) &date_time_, size);
    }
    void deserialize(const char *buffer, size_t size) {
        memcpy((void*) & date_time_, buffer, size);
    }
    size_t get_serialize_size() const { return 9; }
    int get_type() const { return MyMessages::FUCK; }

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
};

class ShitMessage : public AbstractSerializableMessage {
public:
    void serialize(char *buffer, size_t size) {
        memcpy(buffer, (void*) &student_, size);
    }
    void deserialize(const char *buffer, size_t size) {
        memcpy((void*) & student_, buffer, size);
    }
    size_t get_serialize_size() const { return 8; }
    int get_type() const { return MyMessages::SHIT; }
private:
    struct Student{
        int id;
        uint8_t age;
    }student_;
};

class MessageFactory : public AbstractMessageFactory{
public:
    std::shared_ptr<AbstractSerializableMessage> build_message(const std::string cmd) {
        if (cmd[0] == (char)0xd1 && cmd[1] == char(0xd2))
            return std::make_shared<FuckMessage>();
        else if (cmd[0] == (char)0xd1 && cmd[1] == char(0xd2))
            return std::make_shared<ShitMessage>();
        else
            return std::make_shared<FuckMessage>();
        return nullptr;
    }
};

class ChecksumChecker : public AbstractCRC
{
public:
    bool is_valid(const char *data, size_t data_size, const char *crc_data, size_t crc_size) const { return true;}
};

class ClientPacket : public AbstractPacketSections {
public:
    ClientPacket(){}
    std::vector<Section*> get_packet_sections() const {
        std::vector<Section*> sections;

        //        std::shared_ptr<HeaderSection> header = std::make_shared<HeaderSection>();
        HeaderSection* header = new HeaderSection();
        header->content = std::string{static_cast<char>(0xaa), static_cast<char>(0xff)};

        CMDSection* cmd = new CMDSection();
        cmd->size_bytes = 2;
        cmd->msg_factory = std::make_shared<MessageFactory>();

        LengthSection* length = new LengthSection();
        length->include = PacketSections::Data;
        length->is_msb = true;
        length->size_bytes = 4;

        DataSection* data = new DataSection();
        CRCSection* crc = new CRCSection();
        crc->crc_checker = std::make_shared<ChecksumChecker>();
        crc->size_bytes = 4;

        sections.push_back(header);
        sections.push_back(cmd);
        sections.push_back(length);
        sections.push_back(data);
        sections.push_back(crc);

        return sections;
    };
    void get_error_packet(PacketErrors error, char *data, size_t size){
        switch (error) {
        case PacketErrors::Wrong_CRC : {
                std::cout << "Wrong CRC" << std::endl;
        }
        case PacketErrors::Wrong_Footer : {
                std::cout << "Wrong Footer" << std::endl;
        }
        default: {
            std::cout << "there is a new error" << std::endl;
        }
        }
    }
};

std::shared_ptr<boost::thread_group> Thread_Clients_;
void thread_for_work_client(/*TCPClient *client*/) {
    int counter = 0;
    while (1) {
//        auto msg = client->get_next_packet();
        std::cout << "find packet: " << counter++ << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void new_connection(TCPClient *client)
{
    auto packet = std::make_shared<ClientPacket>();
    client->set_extractor(packet.get());
    boost::thread client_thread(thread_for_work_client);
    Thread_Clients_->add_thread(&client_thread);
    std::cout << "id: " << client->get_client_id() << " port: " << client->get_port() << " ip: " << client->get_ip() << std::endl;
}

void start_tcp_server()
{
    auto tcp_server = std::make_shared<TCPServer>(SERVER_PORT);
    tcp_server->notify_me_for_new_connection(std::bind(new_connection, std::placeholders::_1));
    tcp_server->start();
    std::cout << "Start tcp server with port 8585" << std::endl;
    while(1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}



void send_data_to_server() {
    TCPClient client(SERVER_IP, SERVER_PORT);
    bool con = false;
    auto buffer = new hp::peripheral::CircularBuffer();
    client.set_buffer(buffer);
    while (!con) {
        con = client.connect();
        if (!con)
            std::cout << "Can't connect to tcp server" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::string data{(char)0xaa, (char)0xff, (char)0xd1, (char)0xd2, (char)0x00, (char)0x00,
                (char)0x00, (char)0x09, (char)0x01, (char)0x02, (char)0x03, (char)0x04,
                (char)0x05, (char)0x06, (char)0x07, (char)0x08, (char)0x09, (char)0xaa,
                (char)0xbb, (char)0xcc, (char)0xdd};
    while (1) {
        if (!client.is_connected())
            break;
        client.send(data.data(), data.length());
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "finished" << std::endl;
//    delete buffer;
}

int main()
{
    Thread_Clients_ = std::make_shared<boost::thread_group>();
    Thread_Clients_->create_thread(start_tcp_server);
    Thread_Clients_->create_thread(send_data_to_server);
    Thread_Clients_->join_all();
}
