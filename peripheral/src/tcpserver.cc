#include "tcpserver.h"
#include <iostream>

namespace hp {
namespace peripheral {

TCPServer::TCPServer(int port)
    : port_(port)
{
    client_number_ = 0;
    is_running_ = false;
    accept_connection_ = true;
}

void TCPServer::start()
{
    is_running_ = true;
    try {
        acceptor_ = boost::make_shared<boost::asio::ip::tcp::acceptor>(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_));
        worker_thread_ = boost::make_shared<boost::thread>(&TCPServer::worker_thread,this);
        accept_connection();
    }  catch (boost::wrapexcept<boost::system::system_error>& exp) {
        std::cout << "error: " << exp.what() << std::endl;
    }
}

void TCPServer::notify_me_for_new_connection(std::function<void (TCPClient*)> func)
{
    client_object_connections_.connect(func);
}

void TCPServer::send_to_all_clients(char *data, size_t size)
{
    for(auto client: all_clients_map_) {
        if (client.second->is_connected()) {
            client.second->send(data, size);
        } else {
            std::cout << "client's closed" << std::endl;
        }
    }
}

//void TCPServer::accept_connection(bool state)
//{
//    accept_connection_ = state;
//}

TCPServer::~TCPServer()
{
    std::cout << "~TCPServer" << std::endl;
    io_context_.stop();
    acceptor_->cancel();
    acceptor_.reset();
    worker_thread_->join();
}

void TCPServer::accept_connection()
{
    auto socket_ = std::make_shared<boost::asio::ip::tcp::socket>(io_context_);

    //    tcp_client_ = std::make_shared<TCPClient>(client_number_, io_context_);
    acceptor_->async_accept(*socket_, boost::bind(&TCPServer::handle_accept, this, socket_, boost::asio::placeholders::error()));
}

//void TCPServer::send_to_all_clients(char *data, size_t size)
//{

//}

//void TCPServer::send_to_client(char *data, size_t size, int id)
//{

//}

void TCPServer::handle_accept(std::shared_ptr<boost::asio::ip::tcp::socket> client, const boost::system::error_code &error)
{
    if(!error && accept_connection_) {
        auto tcp_client = std::make_shared<TCPClient>(client);
        all_clients_map_[tcp_client->get_id()] = tcp_client;
        client_object_connections_(tcp_client.get());
        client_number_++;
        accept_connection();
    }
}

void TCPServer::worker_thread()
{
    boost::system::error_code ec;
    io_context_.run(ec);
}

} // namespace peripheral
} // namespace hp

