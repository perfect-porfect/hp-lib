#include <iostream>
#include "tcp_server.h"
#include "hp/common/buffer/circular_buffer.h"
#include "hp/common/buffer/buffer_template.h"
#include "hp/common/buffer/fast_buffer.h"
#include "abstract_ip.h"


using namespace hp::peripheral;

#define SERVER_PORT (8585)
#define SERVER_IP   "127.0.0.1"

void print_send(TCPClient *client);
void print_receive(TCPClient *client);
std::shared_ptr<boost::thread_group> Thread_Clients_;
std::vector<std::thread*> clients_thread;;
std::string send_data;
const uint size_data = 110;

void thread_for_work_client(TCPClientShared client) {
    //    std::this_thread::sleep_for(std::chrono::seconds(1)); //TODO(HP) this is a bug for init client from server
    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        client->send(send_data.data(), send_data.size());
        //        std::cout << "send from client: " << send_data.size() << std::endl;

    }
}

void date_received(const char * data, size_t size, uint32_t id) {
    std::cout << "#" << id << " read: " << size << std::endl;
}


void new_connection(TCPClientShared client)
{
    client->dont_buffer_notify_me_data_received(std::bind(date_received, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    //    printer_thread_ = new std::thread(std::bind(print_receive, client));
    auto thread = new std::thread(thread_for_work_client, client);
    clients_thread.push_back(thread);
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
    auto client = std::make_shared<TCPClient>(SERVER_IP, SERVER_PORT);
    bool con = false;

    while (!con) {
        con = client->connect();
        if (!con)
            std::cout << "Can't connect to tcp server" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }


    //    print_thread_ = new std::thread(std::bind(print_send, client));
    while (1) {
        //        client->async_send(send_data.data(), send_data.length(), std::bind(send_notify, std::placeholders::_1));
        client->send(send_data.data(), send_data.length());
        std::cout << "#" << client->get_client_id() << " write: " << send_data.size() << std::endl;
        client->get_all_bytes();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //        counter++;
    }
}

int main()
{
    send_data.resize(size_data);
    std::memset(&send_data[0], 0xaa, size_data);

    Thread_Clients_ = std::make_shared<boost::thread_group>();
    Thread_Clients_->create_thread(start_tcp_server);

    for (int i = 0 ; i < 10000; i++) {
        Thread_Clients_->create_thread(send_data_to_server);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    Thread_Clients_->join_all();
}
