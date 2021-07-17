#pragma once

#include <mutex>
#include <memory>
#include <memory.h>
#include <condition_variable>
#include <vector>
#include "abstract_buffer.h"

namespace hp {
namespace peripheral {

class CircularBufferFast : public AbstractBuffer
{
public:
    CircularBufferFast(uint32_t size_bytes = 4096);

    inline uint32_t remain_bytes(void);
    void set_size(uint32_t size);
    BufferError read(uint8_t *data, const uint32_t len, const uint32_t timeout = 0) override;
    BufferError write(const uint8_t *data, const uint32_t len) override;
    uint8_t read_next_byte() override;
    virtual ~CircularBufferFast();
private:
    inline unsigned count(void);
    inline bool check_free_space(const uint32_t len);

    uint32_t size_;
    bool destructor_call_;

//    std::mutex m;
    std::condition_variable read_signal_;
//    uint8_t* buf_;
//    uint32_t head_;
//    uint32_t tail_;
//    std::vector<char> buffer_;
//    std::atomic<uint32_t> read_index_;
//    std::atomic<uint32_t> write_index_;

    // AbstractBuffer interface
public:
    std::string get_all_bytes();
};

} // namespace peripheral
} // namespace hp
