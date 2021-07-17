#pragma once

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <string>
#include <vector>

#include "hp/common/buffer/abstract_buffer.h"
#include "abstract_ip.h"
#include "hp/common/buffer/buffer_template.h"
#include "message_extractor.h"

typedef boost::shared_ptr<boost::asio::serial_port> SerialPortShared;

#define SERIAL_PORT_READ_BUF_SIZE 256
namespace hp {
namespace peripheral {

class SerialPort
{

public:
	SerialPort(void);
    ~SerialPort(void);

    bool start(const std::string &port, int baud_rate=115200);
    void stop();

    void set_buffer(AbstractBuffer* buffer);
    void set_buffer_size(uint32_t size);

    void extract_messages(AbstractPacketSections *extractor);
    std::shared_ptr<AbstractSerializableMessage> get_next_packet();
    BufferError get_next_bytes(uint8_t *data, const uint32_t len, const uint32_t timeout_ms = 0);
    uint8_t get_next_byte();
    std::string get_all_bytes();
    uint32_t get_remain_bytes() const;

    void notify_me_when_disconnected(std::function<void (int) >func);
    void dont_buffer_notify_me_data_received(std::function<void (const char * data, size_t size)> func);


    int send(const std::string &buf);
    int send(const char *buf, const int &size);
    void async_send(const std::string &buf, std::function<void (int)> func);
    void async_send(const char *buf, const int &size, std::function<void (int)> func);

	static std::vector<std::string> get_port_names();
    static void print_ports();
    static bool is_port_accessible(std::string &port);

    void print_send_receive_rate();
    void print_send_rate();
    void print_receive_rate();
private:
    void extract_message();
    void async_read_some();
    void handle_read_data(const boost::system::error_code& ec, size_t bytes_transferred);

        bool is_running_;
    boost::asio::io_service io_service_;
    SerialPortShared port_;
    boost::mutex mutex_;
    AbstractBuffer* buffer_;
    AbstractPacketSections* msg_extractor_;
    uint32_t buffer_size_;
    bool is_set_buffer_;
    bool is_set_extractor_;
    Buffer<std::shared_ptr<AbstractSerializableMessage>> messages_buffer_;
    boost::signals2::signal <void (const char* data, size_t size)> data_received_connections_;
    boost::signals2::signal <void (size_t)> disconnect_connections_;
    bool is_buffered_data_;
    std::array<uint8_t, SERIAL_PORT_READ_BUF_SIZE> data_;
    boost::thread_group thread_group_;
    boost::shared_ptr<MessageExtractor> serial_message_extractor_;
};

} // namespace peripheral
} // namespace hp
