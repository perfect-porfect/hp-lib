#pragma once

#include <mutex>
#include <memory>
#include <memory.h>

#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include "abstract_buffer.h"

namespace hp {
namespace peripheral {

class FastBuffer : public AbstractBuffer
{

public:
    FastBuffer(uint32_t size = 2 * 1024 * 1024);

    BufferError read(uint8_t *data, const uint32_t len, const uint32_t timeout = 0);
    BufferError write(const uint8_t *data, const uint32_t len);
    uint8_t read_next_byte();

    ~FastBuffer();

private:
    char *input_buffer_;
    uint32_t volatile buffer_write_index_;
    uint32_t volatile buffer_read_index_;
    bool finish_waiting_;
    uint32_t buffer_size_;

    // AbstractBuffer interface
public:
    std::string get_all_bytes();
};

} // namespace peripheral
} // namespace hp
