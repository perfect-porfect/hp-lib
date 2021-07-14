#include <iostream>
#include "tcp_server.h"
#include "circular_buffer.h"
#include "buffer_template.h"
#include "abstract_peripheral.h"
#include "fast_buffer.h"


//The wire format of a simple packet
//                        0            2            6                 10
//                        +------------+------------+------------------+------------------+--------------+------------+
// Name:                  |   Header   |    CMD     |       Len        |       Data       |     CRC      |   Footer   |
//                        +------------+------------+------------------+------------------+--------------+------------+
// Bytes:                 |   2 Bytes  |  2 Bytes   |     4 Bytes      |    Len value     |    4 Bytes   |   2 Bytes  |
//                        +------------+------------+------------------+------------------+--------------+------------+
// Packet Detail:         | 0xaa 0xff  | SHIT, FUCK | First byte's msb |  Serialize Data  | Sum of Bytes |  0xaa 0xcc |
//                        +------------+------------+------------------+------------------+--------------+------------+


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
    FuckMessage() { msg_size_ = sizeof (date_time_);}
    void serialize(char *buffer, size_t size) {
        memcpy(buffer, (void*) &date_time_, size);
    }
    void deserialize(const char *buffer, size_t size) {
        memcpy((void*) & date_time_, buffer, size);
    }
    size_t get_serialize_size() const { return sizeof(DateTime); }
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
public:
    ClientPacket(){}
    std::vector<Section*> get_packet_sections() const {
        std::vector<Section*> sections;

        //        std::shared_ptr<HeaderSection> header = std::make_shared<HeaderSection>();
        HeaderSection* header = new HeaderSection();
        header->content = std::string{static_cast<char>HEADER1, static_cast<char>HEADER2};

        CMDSection* cmd = new CMDSection();
        cmd->size_bytes = CMD_SIZE;
        cmd->msg_factory = std::make_shared<MessageFactory>();

        LengthSection* length = new LengthSection();
        length->include = PacketSections::Data;
        length->is_first_byte_msb = true;
        length->size_bytes = LENGTH_SIZE;

        DataSection* data = new DataSection();
        CRCSection* crc = new CRCSection();
        crc->crc_checker = std::make_shared<ChecksumChecker>();
        crc->size_bytes = CRC_SIZE;

        FooterSection* footer = new FooterSection();
        footer->content = std::string{static_cast<char>FOOTER1, static_cast<char>FOOTER2};
        sections.push_back(header);
        sections.push_back(cmd);
        sections.push_back(length);
        sections.push_back(data);
        sections.push_back(crc);
        sections.push_back(footer);

        return sections;
    };

    WhatFuckingDo get_error_packet(PacketErrors error, const char *data, size_t size){
        switch (error) {
        case PacketErrors::Wrong_CRC : {
            std::cout << "Wrong CRC" << std::endl;
            return WhatFuckingDo::Find_Header;
        }
        case PacketErrors::Wrong_Footer : {
            std::cout << "Wrong Footer" << std::endl;
            return WhatFuckingDo::Find_Header;
        }
        default: {
            std::cout << "There is a new error" << std::endl;
            return WhatFuckingDo::Find_Header;
        }
        }
    }
};

std::shared_ptr<std::thread> Client_Thread;
std::shared_ptr<boost::thread_group> Thread_Clients_;

void thread_for_work_client(TCPClient *client) {
    int counter = 0;
    while (1) {
        auto msg = client->get_next_packet();
        if (msg->get_type() == MyMessages::FUCK) {
            auto fuck_msg = std::dynamic_pointer_cast<FuckMessage>(msg);
            if (counter % 1000 == 0)
                std::cout << "#" << counter << " FUCK_MSG time_date: " << fuck_msg->get_date_time().year << "-" << (int)fuck_msg->get_date_time().month << "-" << (int)fuck_msg->get_date_time().day << " "
                          << (int)fuck_msg->get_date_time().hour << ":" << (int)fuck_msg->get_date_time().min << ":" << (int)fuck_msg->get_date_time().sec << std::endl;
        } else if (msg->get_type() == MyMessages::SHIT) {
            auto shit_msg = std::dynamic_pointer_cast<ShitMessage>(msg);
            std::cout << "#" << counter << " SHIT_MSG  student id: " << shit_msg->get_student().id << "  age: " << (int)shit_msg->get_student().age << std::endl;
        }
        counter++;
    }
}

void new_connection(TCPClient *client)
{
    //    auto buffer = std::make_shared<hp::peripheral::FastBuffer>();
    //    client->set_buffer(buffer.get());
    auto packet = std::make_shared<ClientPacket>();
    client->set_extractor(packet.get());
    Client_Thread = std::make_shared<std::thread>(thread_for_work_client, client);
    std::cout << "id: " << client->get_client_id() << " port: " << client->get_port() << " ip: " << client->get_ip() << std::endl;
}

void start_tcp_server()
{
    auto tcp_server = std::make_shared<TCPServer>(SERVER_PORT);
    tcp_server->notify_me_for_new_connection(std::bind(new_connection, std::placeholders::_1));
    tcp_server->start();
    std::cout << "Start tcp server with port 8585" << std::endl;
    while(1) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void send_data_to_server() {
    TCPClient client(SERVER_IP, SERVER_PORT);
    bool con = false;

    while (!con) {
        con = client.connect();
        if (!con)
            std::cout << "Can't connect to tcp server" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::vector<std::string> all_data;
    std::string fuck_message_ok      {(char)HEADER1, (char)HEADER2, (char)0xd1,    (char)0xd2, (char)0x00, (char)0x09,
                (char)0x15,    (char)0x00,    (char)0x00,    (char)0x00, (char)0x05, (char)0x06, (char)0x07, (char)0x08, (char)0x09,
                (char)0x38,    (char)0x00,    (char)FOOTER1, (char)FOOTER2 };

    std::string shit_message_ok      {(char)HEADER1, (char)HEADER2, (char)0xd2,    (char)0xd3, (char)0x00, (char)0x05,
                (char)0x04,    (char)0x00,    (char)0x00,    (char)0x00, (char)0x20,
                (char)0x24,    (char)0x00,    (char)FOOTER1, (char)FOOTER2 };

    std::string shit_message_crc     {(char)HEADER1, (char)HEADER2, (char)0xd2,    (char)0xd3, (char)0x00, (char)0x05,
                (char)0x04,    (char)0x00,    (char)0x00,    (char)0x00, (char)0x20,
                (char)0x22,    (char)0x00,    (char)FOOTER1, (char)FOOTER2 };

    std::string shit_message_footer  {(char)HEADER1, (char)HEADER2, (char)0xd2,    (char)0xd3, (char)0x00, (char)0x05,
                (char)0x04,    (char)0x00,    (char)0x00,    (char)0x00, (char)0x20,
                (char)0x24,    (char)0x00 };

    all_data.push_back(fuck_message_ok);
    //    all_data.push_back(shit_message_ok);
    //    all_data.push_back(shit_message_crc);
    //    all_data.push_back(shit_message_footer);
    //    all_data.push_back(fuck_message_ok);

    //    while (1) {
    //        if (!client.is_connected())
    //            break;
    //        for(auto data : all_data) {
    for (int i = 0 ; i < 10000; i++) {
        /* int result = */client.send(fuck_message_ok.data(), fuck_message_ok.length());
        //        if (result != int(fuck_message_ok.length()))
        //            std::cout << "Cant send data to server" << std::endl;
    }
    //            std::this_thread::sleep_for(std::chrono::seconds(40));
    //            return;
    //            std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //        }
    //    }
    std::cout << "finished" << std::endl;
    while(1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

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
