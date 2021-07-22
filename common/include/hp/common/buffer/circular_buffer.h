#pragma once

#include <mutex>
#include <memory>
#include <memory.h>
#include <condition_variable>
#include <vector>
#include "abstract_buffer.h"

namespace hp {
namespace peripheral {

class CircularBuffer : public AbstractBuffer
{
public:
    CircularBuffer(uint32_t size_bytes = 4096);

    inline uint32_t remain_bytes(void);
    void set_size(uint32_t size);
    BufferError read(uint8_t *data, const uint32_t len, const uint32_t timeout = 0) override;
    BufferError write(const uint8_t *data, const uint32_t len) override;
    char read_next_byte() override;
    std::string get_all_bytes() override;
    uint32_t get_remain_bytes() override;
    virtual ~CircularBuffer();
    BufferError read(char *data, const uint32_t len, const uint32_t timeout) override;
    BufferError write(const char *data, const uint32_t len) override;
    void clear_buffer() override;
    void erase_buffer() override;
    void set_new_buffer_size(size_t size) override;
private:
    inline unsigned count(void);
    inline bool check_free_space(const uint32_t len);

    uint32_t size_;
    bool destructor_call_;

    std::mutex m;
    std::condition_variable read_signal_;
    std::vector<char> buf_;
    uint32_t head_;
    uint32_t tail_;
};

} // namespace peripheral
} // namespace hp
