#include <iostream>
#include "tcpserver.h"
#include "circular_buffer.h"
#include "buffer_template.h"
#include <abstract_peripheral.h>
#include <csignal>

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
    auto test = tcp_client->connect();
    if (test)
        std::cout << "Tcp client connected" << std::endl;
    async_send(tcp_client);
    //    blocking_send(tcp_client);
}

boost::thread_group thread_client;
auto tcp_server = std::make_shared<TCPServer>(8585);
void thread_for_work_client(TCPClient *client) {
    int counter = 0;
    while (1) {
        auto msg = client->get_next_packet();
        std::cout << "find packet: " << counter++ << std::endl;
    }
}

void received_data(const char* data, size_t size, uint32_t id)
{
    std::cout << "received size: " << size << std::endl;
}

void new_connection(TCPClient *client)
{
    auto packet = std::make_shared<ClientPacket>();
    client->set_extractor(packet.get());
//    client->dont_buffer_notify_me_data_received(received_data);
    std::cout << "id: " << client->get_client_id() << " port: " << client->get_port() << " ip: " << client->get_ip() << std::endl;
    thread_client.create_thread(std::bind(thread_for_work_client, client));
}

void start_tcp_server()
{
    tcp_server->notify_me_for_new_connection(std::bind(new_connection, std::placeholders::_1));
    tcp_server->start();
    std::cout << "Start tcp server with port 8585" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
//    tcp_server->stop();
    //    std::this_thread::sleep_for(std::chrono::seconds(10));
    //    tcp_server->start();

//    std::cout << "stop server" << std::endl;
    while(1) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        //        break;
    }
}


void signal_handler( int signal_num ) {
   std::cout << "The interrupt signal is (" << signal_num << "). \n";
   thread_client.join_all();

   // terminate program
   exit(signal_num);
}

int main()
{
    signal(SIGINT, signal_handler);
    start_tcp_server();
    //    start_tcp_client();
    //    Buffer<std::shared_ptr<TCPClient>> messages_buffer_(12);
}
