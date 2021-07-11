#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>

#include <boost/mpl/size.hpp>
#include "abstract_peripheral.h"
#include "abstract_buffer.h"
#include "message_extractor.h"
#include "buffer_template.h"

#define MAX_LENGTH (256 * 4)

typedef std::shared_ptr<boost::asio::ip::tcp::socket> TCPSocketShared;

namespace hp {
namespace peripheral {

enum class TCPError{
    Server
};

class TCPClient {
public:
    TCPClient(TCPSocketShared socket);
    TCPClient(std::string ip, short port);


    bool connect();
    void disconnect();
    size_t send(const char* data, const uint32_t size);
    void async_send(const char* data, const uint32_t size, std::function<void (int)> func);

    std::string get_ip() const;
    short get_port() const;
    int get_client_id() const;
    bool is_connected() const;

    boost::signals2::connection notify_me_when_disconnected(std::function<void (int) >func);
    boost::signals2::connection dont_buffer_notify_me_data_received(std::function<void (const char * data, size_t size, uint32_t id)> func);

    void set_buffer(AbstractBuffer *buffer);
    void set_extractor(AbstractPacketSections *extractor);
    void set_buffer_size(uint32_t size_bytes);
    std::shared_ptr<AbstractSerializableMessage> get_next_packet();
    BufferError get_next_bytes(uint8_t *data, const uint32_t len, const uint32_t timeout_ms = 0);
    uint8_t get_next_byte();
    std::string get_all_bytes();
    uint32_t get_remain_bytes() const;


    void start_print_send_receive_rate();
    ~TCPClient();
private:
    void initialize();
    void handle_read_data(const boost::system::error_code error, const size_t bytes_transferred);
    void io_context_thread();
    void extract_message();

    std::string ip_;
    short port_;
    uint32_t buffer_size_;
    long int receive_size_;
    bool is_connected_;
    int id_;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    boost::shared_ptr<boost::asio::io_context::work> work_;

    std::array<uint8_t, MAX_LENGTH> data_;
    boost::signals2::signal <void (const char* data, size_t size, uint32_t id)> data_received_connections_;
    boost::signals2::signal <void (int)> disconnect_connections_;

    long int send_size_;

    Buffer<std::shared_ptr<AbstractSerializableMessage>> messages_buffer_;
    boost::thread_group thread_group_;
    boost::asio::ip::tcp::endpoint endpoint_;
    boost::asio::io_context io_context_;

    static std::atomic<int> ID_Counter_;
    bool is_buffer_data_;
    bool buffer_is_mine_;
    AbstractBuffer* buffer_;
    AbstractPacketSections* msg_extractor_;
    std::shared_ptr<MessageExtractor> tcp_message_extractor_;
};

} // namespace peripheral
} // namespace hp

typedef std::shared_ptr<hp::peripheral::TCPClient> TCPClientShared;

#endif // TCPCLIENT_H
