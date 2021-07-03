#include "circular_buffer_fast.h"
#include <iostream>
#include <thread>

namespace hp {
namespace peripheral {

CircularBufferFast::CircularBufferFast(uint32_t size_bytes)
    : size_(size_bytes) , destructor_call_(false)
{
//    head_ = tail_ = 0;
//    buf_ = new uint8_t[size_];
////  buf_.resize(size_bytes);
}

CircularBufferFast::~CircularBufferFast()
{
//    destructor_call_ = true;
//    read_signal_.notify_all();
//    delete[] buf_;
//    std::cout << "buffer distructor" << std::endl;
}

uint32_t CircularBufferFast::remain_bytes()
{
//    uint32_t val;
//    if (head_ >= tail_)
//        val = (size_ - 1) - (head_ - tail_);
//    else
//        val = tail_ - head_ - 1;
//    return val;
}

BufferError CircularBufferFast::read(uint8_t *data, const uint32_t len, const uint32_t timeout)
{
//    BufferError ret_val = BufferError::BUF_NOERROR;
//    uint32_t len_data = len;
//    std::unique_lock<std::mutex> lk(m);
//    if (timeout > 0)
//    read_signal_.
//        read_signal_.wait_for(lk, std::chrono::milliseconds(timeout), [&] { return check_free_space(len_data); });
//    else
//        read_signal_.wait(lk, [&] {return check_free_space(len_data);});

//    if (count() < len_data || destructor_call_)
//        return BufferError::BUF_TIMEOUT;

//    while (len_data > 0) {
//        // don't copy beyond the end of the buffer
//        uint32_t c = std::min(len_data, size_ - tail_);
//        memcpy(data, buf_ + tail_, c);
//        data += c;
//        len_data -= c;
//        tail_ = (tail_ + c) % size_;
//    }
//    return ret_val;
}

BufferError CircularBufferFast::write(const uint8_t *data, const uint32_t len)
{
//    BufferError ret_val = BufferError::BUF_NOERROR;
//    uint32_t len_data = len;
//    std::unique_lock<std::mutex> lk(m);
//    bool overflow = len_data > remain_bytes();

//    while (len_data > 0) {
//        // don't copy beyond the end of the buffer
//        uint32_t c = std::min(len_data, size_ - head_);
//        memcpy(buf_ + head_, data, c);
//        len_data -= c;
//        data += c;
//        head_ = (head_ + c) % size_;
//    }

//    if (overflow) {
//        tail_ = (head_ + 1) % size_;
//        ret_val = BufferError::BUF_OVERFLOW;
//        std::cout << "overflow happen" << std::endl;
//    }

//    read_signal_.notify_all();

//    return ret_val;
}

uint8_t CircularBufferFast::read_next_byte()
{
//    uint8_t data;
//    uint8_t *data_ptr = &data;
//    read(data_ptr, 1);
//    return data;
}

void CircularBufferFast::set_size(uint32_t size)
{
//    std::unique_lock<std::mutex> lk(m);
//    size_ = size;
//    buf_.resize(size);
}

unsigned CircularBufferFast::count()
{
//    return (size_ - 1) - remain_bytes();
}

inline bool CircularBufferFast::check_free_space(const uint32_t len)
{
//    if (!destructor_call_)
//        return (count() >= len);
//    else
//        return true;
}

} // namespace peripheral
} // namespace hp
