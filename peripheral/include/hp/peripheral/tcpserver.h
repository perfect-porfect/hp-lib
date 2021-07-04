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
    TCPServer(boost::asio::io_context &io_context, int port);
    void start();
    void notify_me_for_new_connection(std::function<void (TCPClient *)> func);
    void send_to_all_clients(char* data, size_t size);
    ~TCPServer();
private:
    void accept_connection();
    void handle_accept();
    void worker_thread();

    int port_;
    bool is_running_;
    bool accept_connection_;
    uint32_t client_number_;
//    std::map<int, TCPClientShared> all_clients_map_;

    boost::asio::io_context& io_context_;
//    boost::shared_ptr<boost::thread> worker_thread_;
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    boost::signals2::signal<void (TCPClient*)> client_object_connections_;
};

} // namespace peripheral
} // namespace hp

#endif // TCPASYNC_H
