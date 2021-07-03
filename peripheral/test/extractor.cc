#include <iostream>
#include "tcpserver.h"
#include "circular_buffer.h"
#include "buffer_template.h"
#include <abstract_peripheral.h>

using namespace hp::peripheral;

class FuckMessage : public AbstractSerializableMessage {
public:
    void serialize(char *buffer, size_t size) { }
    void deserialize(const char *buffer, size_t size) { }
    size_t get_serialize_size() { return 4; }
};


class MessageFactory : public AbstractMessageFactory{
public:
    std::shared_ptr<AbstractSerializableMessage> build_message(const std::string cmd) {
        if (cmd[0] == (char)0xd1 && cmd[1] == char(0xd2))
            return std::make_shared<FuckMessage>();
        return nullptr;
    }
};

class ClientPacket : public AbstractRawExtractor {
public:
    ClientPacket(){}
    PacketSections get_packet_len_include() const { return PacketSections::Data/* | PacketSections::Footer*/; }
    int get_header_len() const         { return 2; }
    int get_packet_len() const         { return 4; }
    int get_footer_len() const         { return 4; }
    int get_cmd_len() const            { return 1; }
    int get_crc_len() const            { return 0; }
    bool is_packet_len_msb() const     { return false; }
    std::string get_header_content() const { return {(char)0xff, (char)0x00}; }
    std::string get_footer_content() const { return {(char)0x00, (char)0xfa}; };
    std::shared_ptr<AbstractMessageFactory> get_messages_factory() const { return std::make_shared<MessageFactory>(); }
    std::shared_ptr<AbstractCRC> get_crc_checker() const { return nullptr; } ;
    std::vector<PacketSections> get_packet_sections() const { std::vector<PacketSections> sections; sections.push_back(PacketSections::Header); sections.push_back(PacketSections::CMD); sections.push_back(PacketSections::Data);
                                                              return sections;}
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
    client->get_next_packet();
    client->start();
}

void start_tcp_server()
{
    auto tcp_server = std::make_shared<TCPServer>(8585);
    tcp_server->notify_me_for_new_connection(std::bind(new_connection, std::placeholders::_1));
    tcp_server->start();
    int counter = 0;
    auto function = std::bind(test, 2, std::placeholders::_1);
    while(1) {
        if (counter++ == 3)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        all_clients[0]->async_send("asdf", 4, function);
    }
}
//CircularBuffer buffer_;
//uint8_t* data = new uint8_t[4];

//void thread_read() {
//    while(1) {
//        buffer_.read(data, 4, 4000);
//    }
//}

//void thread_write() {
//    std::this_thread::sleep_for(std::chrono::seconds(2));
//    std::cout << "write1" << std::endl;
//    buffer_.write((uint8_t*)"asdf", 4);
//    std::this_thread::sleep_for(std::chrono::seconds(2));
//    std::cout << "write2" << std::endl;
//    buffer_.write((uint8_t*)"A", 1);
//}

//enum ENUM
//{
//    ONE     = 0x01,
//    TWO     = 0x02,
//    THREE   = 0x04,
//    FOUR    = 0x08,
//    FIVE    = 0x10,
//    SIX     = 0x20
//};

//ENUM operator | ( ENUM lhs, ENUM rhs )
//{
//    // Cast to int first otherwise we'll just end up recursing
//    return static_cast< ENUM >( static_cast< int >( lhs ) | static_cast< int >( rhs ) );
//}

//ENUM enumTest( ENUM v )
//{
//    return v;
//}

//int main( int argc, char **argv )
//{
//    // Valid calls to enumTest
//    ENUM v = enumTest( ONE | TWO | FIVE );
//    std::cout << "v: " << v << std::endl;
//    enumTest( TWO | THREE | FOUR | FIVE );
//    enumTest( ONE | TWO | THREE | FOUR | FIVE | SIX );
//    ENUM test = TWO | THREE;
//    return 0;
//}


int main()
{
    //    std::thread read_thread(thread_read);
    //    std::thread write_thread(thread_write);
    //    read_thread.join();
        start_tcp_server();
    //    start_tcp_client();
//    Buffer<std::shared_ptr<TCPClient>> messages_buffer_(12);

}
