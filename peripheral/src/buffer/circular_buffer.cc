#include "circular_buffer.h"
#include <iostream>
#include <thread>

namespace hp {
namespace peripheral {

CircularBuffer::CircularBuffer(uint32_t size_bytes)
    : size_(size_bytes) , destructor_call_(false)
{
    head_ = tail_ = 0;
    buf_ = new uint8_t[size_];
//  buf_.resize(size_bytes);
}

CircularBuffer::~CircularBuffer()
{
    destructor_call_ = true;
    read_signal_.notify_all();
    delete[] buf_;
    std::cout << "buffer distructor" << std::endl;
}

uint32_t CircularBuffer::remain_bytes()
{
    uint32_t val;
    if (head_ >= tail_)
        val = (size_ - 1) - (head_ - tail_);
    else
        val = tail_ - head_ - 1;
    return val;
}

BufferError CircularBuffer::read(uint8_t *data, const uint32_t len, const uint32_t timeout)
{
    BufferError ret_val = BufferError::BUF_NOERROR;
    uint32_t len_data = len;
    std::unique_lock<std::mutex> lk(m);
    if (timeout > 0)
        read_signal_.wait_for(lk, std::chrono::milliseconds(timeout), [&] {static int counter = 0; std::cout << "#" << counter++ << " fuck" << std::endl; return check_free_space(len_data); });
    else
        read_signal_.wait(lk, [&] {return check_free_space(len_data);});

    if (count() < len_data || destructor_call_)
        return BufferError::BUF_TIMEOUT;
    std::cout << "timeout" << std::endl;
    while (len_data > 0) {
        // don't copy beyond the end of the buffer
        uint32_t c = std::min(len_data, size_ - tail_);
        memcpy(data, buf_ + tail_, c);
        data += c;
        len_data -= c;
        tail_ = (tail_ + c) % size_;
    }
    return ret_val;
}

BufferError CircularBuffer::write(const uint8_t *data, const uint32_t len)
{
    BufferError ret_val = BufferError::BUF_NOERROR;
    uint32_t len_data = len;
    std::unique_lock<std::mutex> lk(m);
    bool overflow = len_data > remain_bytes();

    while (len_data > 0) {
        // don't copy beyond the end of the buffer
        uint32_t c = std::min(len_data, size_ - head_);
        memcpy(buf_ + head_, data, c);
        len_data -= c;
        data += c;
        head_ = (head_ + c) % size_;
    }

    if (overflow) {
        tail_ = (head_ + 1) % size_;
        ret_val = BufferError::BUF_OVERFLOW;
        std::cout << "overflow happen" << std::endl;
    }

    read_signal_.notify_all();

    return ret_val;
}

uint8_t CircularBuffer::read_next_byte()
{
    uint8_t data;
    uint8_t *data_ptr = &data;
    read(data_ptr, 1);
    return data;
}

void CircularBuffer::set_size(uint32_t size)
{
    std::unique_lock<std::mutex> lk(m);
    size_ = size;
//    buf_.resize(size);
}

unsigned CircularBuffer::count()
{
    return (size_ - 1) - remain_bytes();
}

inline bool CircularBuffer::check_free_space(const uint32_t len)
{
    if (!destructor_call_)
        return (count() >= len);
    else
        return true;
}

} // namespace peripheral
} // namespace hp
