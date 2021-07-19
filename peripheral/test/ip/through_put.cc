#include <iostream>
#include "tcp_server.h"
#include "hp/common/buffer/circular_buffer.h"
#include "hp/common/buffer/buffer_template.h"
#include "hp/common/buffer/fast_buffer.h"
#include "abstract_ip.h"


//The wire format of a simple packet
//                        0            2            6                 10
//                        +------------+------------+------------------+------------------+--------------+------------+
// Name:                  |   Header   |    CMD     |       Len        |       Data       |     CRC      |   Footer   |
//                        +------------+------------+------------------+------------------+--------------+------------+
// Bytes:                 |   2 Bytes  |  2 Bytes   |     4 Bytes      |    Len value     |    4 Bytes   |   2 Bytes  |
//                        +------------+------------+------------------+------------------+--------------+------------+
// Packet Detail:         | 0xaa 0xff  | SHIT, FUCK | First byte's msb |  Serialize Data  | Sum of Bytes |  0xaa 0xcc |
//                        +------------+------------+------------------+------------------+--------------+------------+
using namespace hp::peripheral;

#define SERVER_PORT (8585)
#define SERVER_IP   "127.0.0.1"


boost::thread_group client_thread;
void print_receive(std::shared_ptr<TCPClient> client);
void data(const char * data, size_t size, uint32_t id) {

}

bool end_of_server_ = true;
auto tcp_server = std::make_shared<TCPServer>(SERVER_PORT);
std::map<int, TCPClientShared> all_clients;

void new_connection(TCPClientShared client)
{
    //    all_clients[client->get_client_id()] = client;
    client->dont_buffer_notify_me_data_received(std::bind(data, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    client->connect();
    client->disconnect();
    client->connect();
    tcp_server->send_to_all_clients((char*)"abc", 3);
    //    client->connect();
    //    client_thread.create_thread(std::bind(print_receive, client));
    std::cout << "id: " << client->get_client_id() << " port: " << client->get_port() << " ip: " << client->get_ip() << std::endl;
}



void start_tcp_server()
{
    tcp_server->notify_me_for_new_connection(std::bind(new_connection, std::placeholders::_1));
    tcp_server->start();
    std::cout << "Start tcp server with port 8585" << std::endl;
    //    while(end_of_server_) {
    std::this_thread::sleep_for(std::chrono::seconds(10));
    //    }
}

void print_send(TCPClient *client)
{
    while(1) {
        client->print_send_rate();
        std::this_thread::sleep_for(std::chrono::milliseconds(1020));
    }
}

void print_receive(std::shared_ptr<TCPClient> client)
{
    while(1) {
        client->print_receive_rate();
        std::this_thread::sleep_for(std::chrono::milliseconds(1020));
    }
}

int main()
{
    auto Thread_Clients_ = std::make_shared<boost::thread_group>();
    Thread_Clients_->create_thread(start_tcp_server);
    Thread_Clients_->join_all();
}
