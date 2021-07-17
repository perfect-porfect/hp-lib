#include "fast_buffer.h"

#include <thread>

namespace hp {
namespace peripheral {

FastBuffer::FastBuffer(uint32_t size)
    : buffer_size_(size)
{
    finish_waiting_ = true;
    buffer_read_index_ = 0;
    buffer_write_index_ = 0;
    input_buffer_ = new char[buffer_size_];
}

BufferError FastBuffer::read(uint8_t *data, const uint32_t len, const uint32_t timeout)
{
    for (uint bytes_no = 0; bytes_no < len; bytes_no++) {
        data[bytes_no] =  read_next_byte();
    }
    return BufferError::BUF_NOERROR;
}

BufferError FastBuffer::write(const uint8_t *data, const uint32_t len)
{
    if (buffer_write_index_ + len <= buffer_size_ ) {
        std::copy(data, data + len, &input_buffer_[buffer_write_index_]);
        buffer_write_index_ += len;
    } else {
        uint can_copy_size = buffer_size_ - buffer_write_index_;
        std::copy(data, data + can_copy_size, &input_buffer_[buffer_write_index_]);
        buffer_write_index_ = 0;
        uint remain = uint(len) - can_copy_size;
        std::copy(data + can_copy_size, data + remain + can_copy_size, &input_buffer_[buffer_write_index_]);
        buffer_write_index_ += remain;
    }
    return BufferError::BUF_NOERROR;
}


uint8_t FastBuffer::read_next_byte()
{
    while (buffer_read_index_ == buffer_write_index_) {
//        if (!finish_waiting_)
//           return '\0';
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    char data = input_buffer_[buffer_read_index_];
    buffer_read_index_++;
    if (buffer_read_index_ == buffer_size_)
        buffer_read_index_ = 0;
    return data;
}

FastBuffer::~FastBuffer()
{
    delete input_buffer_;
}

std::string FastBuffer::get_all_bytes()
{

}

} // namespace peripheral
} // namespace hp


