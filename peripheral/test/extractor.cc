#include <iostream>
#include "tcpserver.h"
#include "circular_buffer.h"
#include "buffer_template.h"
#include <abstract_peripheral.h>

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
        uint8_t year;
        uint8_t month;
        uint8_t day;
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

    // AbstractRawExtractor interface
public:
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
};




void test(int index, int size)
{
    std::cout << "index : " << index  << " size: " << size << std::endl;
}

void blocking_send(std::shared_ptr<TCPClient> tcp_client)
{
    std::string data = "hello\n";
    int counter = 0;
    while(1) {
        int bytes = tcp_client->send(data.data(), data.size());
        if (int(data.size()) == bytes)
            break;
        if (counter  == 1) {
            tcp_client->disconnect();
        }
        counter++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void async_send(std::shared_ptr<TCPClient> tcp_client)
{
    std::string data = "hello\n";
    int counter = 0;
    auto function = std::bind(test, 2, std::placeholders::_1);
    while(1) {
        tcp_client->async_send(data.data(), data.size(), function);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (counter  == 1) {
            tcp_client->disconnect();
            break;
        }
        counter++;
    }
}

void start_tcp_client()
{
    auto tcp_client =  std::make_shared<TCPClient>("127.0.0.1", 8585);
    auto test = tcp_client->start();
    if (test)
        std::cout << "Tcp client connected" << std::endl;
    async_send(tcp_client);
    //    blocking_send(tcp_client);
}
std::map<int, TCPClient*> all_clients;


void new_connection(TCPClient *client)
{
    all_clients[client->get_id()] = client;
    std::cout << "id: " << client->get_id() << " port: " << client->get_port() << " ip: " << client->get_ip() << std::endl;
    auto extractor = std::make_shared<ClientPacket>();
    client->set_extractor(extractor);
    auto msg = client->get_next_packet();
    int ad = 0;
}

void start_tcp_server()
{
    auto tcp_server = std::make_shared<TCPServer>(8585);
    tcp_server->notify_me_for_new_connection(std::bind(new_connection, std::placeholders::_1));
    tcp_server->start();
    //    int counter = 0;
    //    auto function = std::bind(test, 2, std::placeholders::_1);
    while(1) {
        //        if (counter++ == 3)
        //            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //        all_clients[0]->async_send("asdf", 4, function);
    }
}

// aa ff d1 d2 00 00 00 66 01 02 03 04 05 06 07 08 09 aa bb cc dd
int main()
{
    start_tcp_server();
    //    start_tcp_client();
    //    Buffer<std::shared_ptr<TCPClient>> messages_buffer_(12);

}
