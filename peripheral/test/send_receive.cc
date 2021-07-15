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
using namespace hp::peripheral;

#define SERVER_PORT (8585)
#define SERVER_IP   "127.0.0.1"

void print_send(TCPClient *client);
void print_receive(TCPClient *client);
std::shared_ptr<std::thread> Client_Thread;
std::shared_ptr<boost::thread_group> Thread_Clients_;
std::thread *printer_thread_;
const int print_counter = 100000;
void thread_for_work_client(TCPClient *client) {
    std::this_thread::sleep_for(std::chrono::seconds(1)); //TODO(HP) this is a bug for init client from server
    uint32_t rec_size = 110 * 1024 * 1024;
    while (1) {
        std::string data;
        data.resize(rec_size);
        client->get_next_bytes((uint8_t*) data.data(), rec_size);
        std::cout << "read: " << rec_size << std::endl;
    }
}


void new_connection(TCPClient *client)
{
    client->set_buffer_size((uint64_t)120 * 1024 * 1024);
    //    printer_thread_ = new std::thread(std::bind(print_receive, client));
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

std::thread* print_thread_;
void print_send(TCPClient *client)
{
    while(1) {
        client->print_send_rate();
        std::this_thread::sleep_for(std::chrono::milliseconds(1020));
    }
}

void print_receive(TCPClient *client)
{
    while(1) {
        client->print_receive_rate();
        std::this_thread::sleep_for(std::chrono::milliseconds(1020));
    }
}


void send_notify(size_t size) {
    std::cout << "send data: " << size << std::endl;
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
    std::string send_data;
    const uint size_data = 1024 * 1024 * 110;
    send_data.resize(size_data);
    std::memset(&send_data[0], 0xaa, size_data);
    int counter = 1;
    //    print_thread_ = new std::thread(std::bind(print_send, client));
    while (1) {
        //        client->async_send(send_data.data(), send_data.length(), std::bind(send_notify, std::placeholders::_1));
        client->send(send_data.data(), send_data.length());
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "write: " << send_data.size() << std::endl;
        //        counter++;
    }
}

int main()
{
    Thread_Clients_ = std::make_shared<boost::thread_group>();
    Thread_Clients_->create_thread(start_tcp_server);
    //    std::this_thread::sleep_for(std::chrono::seconds(3));
    Thread_Clients_->create_thread(send_data_to_server);
    Thread_Clients_->join_all();
    Client_Thread->join();
}
