#include "tcp_client.h"
#include <iostream>
#include <boost/asio/detail/posix_thread.hpp>
#include "hp/common/buffer/fast_buffer.h"
#include "hp/common/buffer/circular_buffer.h"

namespace hp {
namespace peripheral {

std::atomic<int> TCPClient::ID_Counter_(0);

TCPClient::TCPClient(TCPSocketShared socket)
    : ip_(socket->remote_endpoint().address().to_string()), port_(socket->remote_endpoint().port()), socket_(socket)
{
    is_connected_ = true;
    initialize();
}

TCPClient::TCPClient(std::string ip, short port)
    : ip_(ip), port_(port)
{
    is_connected_ = false;
    work_ = boost::make_shared<boost::asio::io_context::work>(io_context_);
    socket_ = std::make_shared<boost::asio::ip::tcp::socket>(io_context_);
    initialize();
}

void TCPClient::set_buffer(AbstractBuffer* buffer)
{
    //TODO(HP): lock this and copy data to new buffer
//    if (buffer_is_mine_) {
//        delete buffer_;
//    } else {

//    }
    buffer_ = buffer;
    buffer_is_mine_ = false;
}

void TCPClient::extract_messages(AbstractPacketSections* extractor)
{
    msg_extractor_ = extractor;
}

void TCPClient::initialize()
{
    buffer_ = nullptr;
    msg_extractor_ = nullptr;
    buffer_is_mine_ = true;
    is_buffered_data_ = true;
    buffer_size_ = 10 * 1024 * 1024;
    receive_size_ = 0;
    send_size_= 0;
    id_ = ID_Counter_;
    ID_Counter_++;
}

bool TCPClient::connect()
{
    if (!is_connected_) {
        boost::system::error_code ec;
        boost::asio::ip::tcp::resolver resolver(io_context_);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), ip_, std::to_string(port_));
        endpoint_ = *resolver.resolve(query);
        auto error = socket_->connect(endpoint_, ec);
        if (error) {
            is_connected_ = false;
            return is_connected_;
        }
        thread_group_.create_thread(boost::bind(&TCPClient::io_context_thread, this));
    }
    if (buffer_ == nullptr) {
        buffer_ = new CircularBuffer(buffer_size_);
        buffer_is_mine_ = true;
    }

    if (msg_extractor_ != nullptr) {
        thread_group_.create_thread(boost::bind(&TCPClient::extract_message, this));
    }
    socket_->async_receive(boost::asio::buffer(data_.data(), data_.size()),
                           boost::bind(&TCPClient::handle_read_data, this, boost::asio::placeholders::error ,boost::asio::placeholders::bytes_transferred));
    is_connected_ = true;
    return is_connected_;
}

void TCPClient::set_buffer_size(uint64_t size_bytes)
{
    //    buffer_->set_size(size_bytes);
    buffer_size_ = size_bytes;
}


size_t TCPClient::send(const char *data, const uint32_t size)
{
    if (is_connected_) {
        send_size_ += size;
        return socket_->send(boost::asio::buffer(data, size));
    }
    return -1;
}

void TCPClient::async_send(const char *data, const uint32_t size, std::function<void (size_t)> func)
{
    if (is_connected_)
        socket_->async_send(boost::asio::buffer(data, size), boost::bind(func, boost::asio::placeholders::bytes_transferred));
}

void TCPClient::handle_read_data(const boost::system::error_code error, const size_t bytes_transferred)
{
    if (!error) {
        if (!is_buffered_data_) {
            data_received_connections_((char*)data_.data(), bytes_transferred, id_);
        } else {
            buffer_->write(data_.data(), bytes_transferred);
        }
        receive_size_ += bytes_transferred;
        socket_->async_receive(boost::asio::buffer(data_.data(), data_.size()),
                               boost::bind(&TCPClient::handle_read_data, this, boost::asio::placeholders::error ,boost::asio::placeholders::bytes_transferred));
    } else {
        if (is_connected_) {
            is_connected_ = false;
            disconnect_connections_(id_);
            std::cout << "disconnect from server" << std::endl;
        }
    }
}

BufferError TCPClient::get_next_bytes(uint8_t *data, const uint32_t len, const uint32_t timeout_ms)
{
    if (msg_extractor_ != nullptr)
        return BufferError::BUF_NODATA;
    auto ret = buffer_->read(data, len, timeout_ms);
    return ret;
}

uint8_t TCPClient::get_next_byte()
{
    uint8_t data;
    uint8_t *data_ptr = &data;
    buffer_->read(data_ptr, 1);
    return data;
}

std::string TCPClient::get_all_bytes()
{
    return buffer_->get_all_bytes();
}

uint32_t TCPClient::get_remain_bytes() const
{

}

void TCPClient::check_line_state()
{

}

void TCPClient::extract_message()
{
    tcp_message_extractor_ = std::make_shared<MessageExtractor>(msg_extractor_, buffer_);
    while(is_connected_) {
        auto msg = tcp_message_extractor_->find_message();
        if (msg == nullptr) {
            std::cout << "fucking null msg" << std::endl;
            exit(1);
        }
        messages_buffer_.write(msg);
    }
}

boost::signals2::connection TCPClient::notify_me_when_disconnected(std::function<void (int)> func)
{
    return disconnect_connections_.connect(func);
}

boost::signals2::connection TCPClient::dont_buffer_notify_me_data_received(std::function<void (const char *, size_t, uint32_t)> func)
{
    is_buffered_data_ = false;
    return data_received_connections_.connect(func);
}

bool TCPClient::is_connected() const
{
    return is_connected_;
}

void TCPClient::disconnect()
{
    socket_->close();
    is_connected_ = false;
}
void TCPClient::print_send_receive_rate()
{
    static int sec = 0;
    if (send_size_ < 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " send " + std::to_string(send_size_) + " Bytes" << std::endl;
    } else if (send_size_ < 1024 * 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " send " + std::to_string((double)send_size_    / (1024) ) + " KB/s  " << std::endl;
    } else if (send_size_ < 1024 * 1024 * 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " send " + std::to_string((double)send_size_    / (1024 * 1024) ) + " MB/s  " << std::endl;
    } else if (send_size_ >= 1024 * 1024 * 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " send " + std::to_string((double)send_size_    / (1024*1024 * 1024) ) + " GB/s  " + std::to_string(send_size_) + " Bytes" << std::endl;
    }

    if (receive_size_ < 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " receive " + std::to_string(receive_size_) + " Bytes" << std::endl;
    } else if (receive_size_ < 1024 * 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " receive " + std::to_string((double)receive_size_ / (1024) ) + " KB/s  " << std::endl;
    } else if (receive_size_ < 1024 * 1024 * 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " receive " + std::to_string((double)receive_size_ / (1024 * 1024) ) + " MB/s  " << std::endl;
    } else if (receive_size_ >= 1024 * 1024 * 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " receive " + std::to_string((double)receive_size_ / (1024*1024 * 1024) ) + " GB/s  " + std::to_string(receive_size_) + " Bytes" << std::endl;
    }

    sec++;
    send_size_ = 0;
    receive_size_ = 0;
}

void TCPClient::print_send_rate()
{
    static int sec = 0;
    if (send_size_ < 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " send " + std::to_string(send_size_) + " Bytes" << std::endl;
    } else if (send_size_ < 1024 * 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " send " + std::to_string((double)send_size_    / (1024) ) + " KB/s  " << std::endl;
    } else if (send_size_ < 1024 * 1024 * 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " send " + std::to_string((double)send_size_    / (1024 * 1024) ) + " MB/s  " << std::endl;
    } else if (send_size_ >= 1024 * 1024 * 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " send " + std::to_string((double)send_size_    / (1024*1024 * 1024) ) + " GB/s  " + std::to_string(send_size_) + " Bytes" << std::endl;
    }
    sec++;

    send_size_ = 0;
}

void TCPClient::print_receive_rate()
{
    static int sec = 0;

    if (receive_size_ < 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " receive " + std::to_string(receive_size_) + " Bytes" << std::endl;
    } else if (receive_size_ < 1024 * 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " receive " + std::to_string((double)receive_size_ / (1024) ) + " KB/s  " << std::endl;
    } else if (receive_size_ < 1024 * 1024 * 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " receive " + std::to_string((double)receive_size_ / (1024 * 1024) ) + " MB/s  " << std::endl;
    } else if (receive_size_ >= 1024 * 1024 * 1024) {
        std::cout << "sec: " << sec << " ID: " << id_ << " receive " + std::to_string((double)receive_size_ / (1024*1024 * 1024) ) + " GB/s  " + std::to_string(receive_size_) + " Bytes" << std::endl;
    }

    sec++;
    send_size_ = 0;
    receive_size_ = 0;
}

void TCPClient::io_context_thread()
{
    io_context_.run();
}

std::string TCPClient::get_ip() const
{
    return ip_;
}

short TCPClient::get_port() const
{
    return port_;
}

int TCPClient::get_client_id() const
{
    return id_;
}

std::shared_ptr<AbstractSerializableMessage> TCPClient::get_next_packet()
{
    return messages_buffer_.get_next_packet();
}

TCPClient::~TCPClient()
{
    if (is_connected_)
        disconnect();
    if (work_ != nullptr)
        work_.reset();
    thread_group_.join_all();
    socket_.reset();
    if (buffer_is_mine_)
        delete buffer_;
}

} // namespace peripheral
} // namespace hp

