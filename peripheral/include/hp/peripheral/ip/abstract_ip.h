#pragma once

#include <memory>
#include <boost/signals2.hpp>

//#include "abstract_buffer.h"

namespace hp {
namespace peripheral {

enum PacketSections {
    Header  = 1 << 0,
    Length  = 1 << 1,
    CMD     = 1 << 2,
    Footer  = 1 << 3,
    CRC     = 1 << 4,
    Data    = 1 << 5,
    Other   = 1 << 6
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
    //!
    virtual bool is_valid(const std::map<PacketSections, std::string>& input_data, const std::string& data) = 0;
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
    virtual size_t get_serialize_size() const = 0;
    virtual int get_type() const = 0;
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



template<class T> inline T operator~ (T a) { return (T)~(int)a; }
template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }


struct Section{
public:
    virtual PacketSections get_type() const = 0;
};

struct CRCSection : public Section {
    uint32_t size_bytes = 0;
    PacketSections include = PacketSections::Data;
    std::shared_ptr<AbstractCRC> crc_checker;
public:
    virtual PacketSections get_type()  const override { return PacketSections::CRC;}
};

struct LengthSection : public Section {
    uint32_t size_bytes = 0;
    PacketSections include = PacketSections::Data;
    bool is_first_byte_msb;
public:
    PacketSections get_type() const { return PacketSections::Length;}
};

struct HeaderSection : public Section {
    std::string content;
public:
    PacketSections get_type() const { return PacketSections::Header;}
};

struct FooterSection : public Section {
    std::string content;
public:
    PacketSections get_type() const { return PacketSections::Footer;}
};

struct CMDSection : public Section {
    uint32_t size_bytes = 0;
    std::shared_ptr<AbstractMessageFactory> msg_factory;
public:
    PacketSections get_type() const { return PacketSections::CMD;}
};

struct DataSection : public Section {
    uint32_t fix_size_bytes = 0;
public:
    PacketSections get_type() const { return PacketSections::Data;}
};

struct Other : public Section {
    uint32_t size_bytes = 0;
    virtual bool is_right() { return false;}
public:
    PacketSections get_type() const { return PacketSections::Data;}
};

enum class PacketErrors{
    //TODO(HP): better way is throw exception and in what explain error (developer dont need to implement these codes)
    Wrong_CRC,
    Wrong_Footer,
    Wrong_CMD,
    Wrong_Length_Include
};

enum class WhatFuckingDo{
    Find_Header,

};

class AbstractPacketSections {
public:
    virtual std::vector<Section*> get_packet_sections() = 0;
    virtual void get_error_packet(PacketErrors error, const std::map<PacketSections, std::string>& packet) = 0;
};

} // namespace peripheral
} // namespace hp
