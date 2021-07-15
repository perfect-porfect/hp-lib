#pragma once
#include <stdint.h>

namespace hp {
namespace peripheral {

enum BufferError {
    BUF_NOERROR = 0,
    BUF_OVERFLOW = 1,
    BUF_NODATA = 2,
    BUF_TIMEOUT = 3,
};

class AbstractBuffer
{
public:
    virtual BufferError read(uint8_t *data, const uint32_t len, const uint32_t timeout = 0) = 0;
    virtual uint8_t read_next_byte() = 0;
    virtual BufferError write(const uint8_t *data, const uint32_t len) = 0;
    virtual std::string get_all_bytes() = 0;
    virtual ~AbstractBuffer() {}
};

template <class Type>
class AbstractTemplateBuffer
{
public:
    virtual void write(Type obj) = 0;
    virtual Type get_next_packet() = 0;
    virtual ~AbstractTemplateBuffer<Type>() {}
};


} // namespace peripheral
} // namespace hp
