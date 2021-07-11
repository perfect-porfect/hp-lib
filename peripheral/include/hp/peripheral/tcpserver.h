#ifndef TCPASYNC_H
#define TCPASYNC_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <memory>
#include <thread>
#include "tcpclient.h"

namespace hp {
namespace peripheral {

class TCPServer
{
public:
    TCPServer(int port);
    void start();
    void notify_me_for_new_connection(std::function<void (TCPClient *)> func);
    void send_to_all_clients(char* data, size_t size);
    void send_to_client(char* data, size_t size, uint32_t id);
    void accept_connection(bool state);
    void dont_buffer_notify_me_data_received(std::function<void (char * data, size_t size, uint32_t id)> func);
    TCPClient *get_client(int id) const;
    ~TCPServer();
private:
    void disconnect(int id);
    void handle_connection();
    void handle_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code &error);
    void worker_thread();

    int port_;
    bool is_running_;
    bool accept_connection_;
    uint32_t client_number_;
    std::map<int, TCPClientShared> all_clients_map_;

    boost::shared_ptr<boost::asio::io_context> io_context_;
    boost::shared_ptr<boost::thread> worker_thread_;
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    boost::signals2::signal<void (TCPClient*)> client_object_connections_;
    std::function<void (char* data, size_t size, uint32_t id)> received_data_func_;

};

} // namespace peripheral
} // namespace hp

#endif // TCPASYNC_H
