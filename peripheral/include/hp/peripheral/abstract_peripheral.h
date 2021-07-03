#pragma once

#include <memory>
#include <boost/signals2.hpp>

#include "abstract_buffer.h"

namespace hp {
namespace peripheral {

enum PacketSections {
    Header  = 0b00000001,
    Length  = 0b00000010,
    CMD     = 0b00000100,
    Footer  = 0b00001000,
    CRC     = 0b00010000,
    Data    = 0b00100000,
    Other   = 0b01000000
};


class AbstractCRC {
public:
    //!
    //! \brief is_valid check data in packet is valid or not.
    //! \param data  include data section in packet
    //! \param data_size data section size in packet
    //! \param crc_data data of crc
    //! \param crc_size size of crc
    //! \return true mean the crc match with data.
    //!
    virtual bool is_valid(const char* data, size_t data_size, const char* crc_data, size_t crc_size) const = 0;
};


//!
//! \brief The AbstractSerializableMessage class is an interface which implement
//! the Message and used for any type of message that can serial and deseriale itself.
//!

class AbstractSerializableMessage {
public :
    //!
    //! \brief serialize method can fill the input buffer by serializing values .
    //!        the values in serializing and deserializing should be same.
    //! \param buffer for containg the serializing data
    //! \param size is used for available buffer size
    //!
    virtual void serialize(char* buffer, size_t size) = 0;
    //!
    //! \brief deserialize method is used for setting properties and members
    //!        from buffer as same method as serialize
    //! \param buffer is used for reading values
    //! \param size the values
    //!
    virtual void deserialize(const char * buffer, size_t size) = 0 ;
    //!
    //! \brief getserialSize method return the size of serializing and deserializing method
    //! \return the size_t value which show the serializ size this function will used when
    //!         packet not include packet_len
    //!
    virtual size_t get_serialize_size() = 0;
    virtual ~AbstractSerializableMessage(){} //! virtual interface destructor
};

class AbstractMessageFactory
{
public:
    //!
    //! \brief build_message is a factory for all messages that packet includes.
    //! \param cmd is a section that use in packet
    //! \return  must return a serializablemessage for save all messages
    //!
    virtual std::shared_ptr<AbstractSerializableMessage> build_message(const std::string cmd) = 0;
};

class AbstractRawExtractor {
public:
    virtual PacketSections get_packet_len_include() const = 0;
    virtual int get_packet_len() const = 0;
    virtual int get_cmd_len() const = 0;
    virtual int get_crc_len() const = 0;

    virtual bool is_packet_len_msb() const = 0;

    virtual std::string get_header_content() const = 0;
    virtual std::string get_footer_content() const = 0;

    virtual std::shared_ptr<AbstractMessageFactory> get_messages_factory() const = 0;
    virtual std::shared_ptr<AbstractCRC> get_crc_checker() const  = 0;
    virtual std::vector<PacketSections> get_packet_sections() const = 0;
    PacketSections operator|( PacketSections lhs, PacketSections rhs ) final
    {
        // Cast to int first otherwise we'll just end up recursing
        return static_cast< PacketSections >( static_cast< int >( lhs ) | static_cast< int >( rhs ) );
    }

};

} // namespace peripheral
} // namespace hp
