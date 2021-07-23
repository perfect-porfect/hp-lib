#include <iostream>
#include "tcp_server.h"
#include "hp/common/buffer/circular_buffer.h"
#include "hp/common/buffer/buffer_template.h"
#include "abstract_ip.h"


using namespace hp::peripheral;

#define SERVER_PORT (8585)
#define SERVER_IP   "127.0.0.1"

void print_send(TCPClient *client);
void print_receive(TCPClient *client);
std::shared_ptr<boost::thread_group> Thread_Clients_;
std::vector<std::thread*> clients_thread;;
std::string send_data;
const uint size_data = 1 * 1024 * 1024;

void thread_for_work_client(TCPClientShared client) {
    //    std::this_thread::sleep_for(std::chrono::seconds(1)); //TODO(HP) this is a bug for init client from server
    std::string rece_data;
    rece_data.resize(size_data);
    while (1) {
        uint32_t sum = 0;
        //        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto ret = client->read_next_bytes(rece_data.data(), rece_data.size());
        for (auto data: rece_data)
            sum += std::pow(1.1, data);
        //        std::cout << "server sum is: " << sum << std::endl;
        if (ret != BUF_NOERROR && ret != BUF_NODATA) {
            std::cout << "whyyyyyy???" << std::endl;
            exit(-1);
        }
        //            std::cout << "Cant get data from: " << client->get_client_id() << std::endl;
        client->send((char*)(&sum), 4);
    }
}

void date_received(const char * data, size_t size, uint32_t id, TCPClientShared client) {

//    auto ret = client->read_next_bytes(rece_data.data(), rece_data.size());
//    for (auto data: rece_data)
//        sum += std::pow(1.1, data);
        std::cout << "receive: " << size << std::endl;
}

void new_connection(TCPClientShared client)
{
    static uint32_t counter = 0;

    client->do_buffer_data(false);
//    client->set_buffer_size(2 * 1024 * 1024);
        client->notify_me_data_received(std::bind(date_received, std::placeholders::_1, std::placeholders::_2, client->get_client_id(), client));
    //    printer_thread_ = new std::thread(std::bind(print_receive, client));
    //auto data = client->read_next_byte(10);
//    auto thread = new std::thread(thread_for_work_client, client);
//    clients_thread.push_back(thread);
    if (counter % 1000 == 0)
        std::cout << "id: " << client->get_client_id() << " port: " << client->get_port() << " ip: " << client->get_ip() << std::endl;
    counter++;
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


bool is_start = false;
void send_data_to_server() {
    auto client = std::make_shared<TCPClient>(SERVER_IP, SERVER_PORT);
    bool con = false;

    while (!con) {
        con = client->connect();
        if (!con)
            std::cout << "Can't connect to tcp server" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    //    std::cout << "connect to server" << std::endl;


    std::string data;
    data.resize(size_data);
    memset(&data[0], 0x02, size_data);
    uint32_t sum_data = 0;
    uint32_t sum_ans = 0x02 * size_data;
    while (1) {
        client->send(data.data(), data.size());
//        auto ret = client->read_next_bytes((char*)(&sum_data), 4, 5000);
//        if (sum_ans != sum_data)
//            std::cout << "sum is: " << sum_data << std::endl;
//        if (ret != BufferError::BUF_NOERROR) {
//            std::cout << "fuck ID: " << client->get_client_id() << std::endl;
//            exit(-1);
//        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    Thread_Clients_ = std::make_shared<boost::thread_group>();
    Thread_Clients_->create_thread(start_tcp_server);
    // cat /proc/sys/kernel/threads-max
    // ulimited -n 999999
    for (int i = 0 ; i < 1; i++) {
        Thread_Clients_->create_thread(send_data_to_server);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }


    Thread_Clients_->join_all();
}
