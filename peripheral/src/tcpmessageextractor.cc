#include "tcpmessageextractor.h"
#include <iostream>

namespace hp {
namespace peripheral {

TCPMessageExtractor::TCPMessageExtractor(std::shared_ptr<AbstractRawExtractor> extractor, std::shared_ptr<AbstractBuffer> buffer)
    : extractor_(extractor), buffer_(buffer)
{
    packet_sections_ = extractor_->get_packet_sections();
    message_factory_ = extractor_->get_messages_factory();
    pkt_len_include_ = extractor_->get_packet_len_include();
    is_pkt_len_msb_  = extractor_->is_packet_len_msb();
    crc_checker_ = extractor_->get_crc_checker();
    footer_len_  = footer_content_.length();
    header_len_  = header_content_.length();
    packet_len_  = extractor_->get_packet_len();
    cmd_len_     = extractor_->get_cmd_len();
    crc_len_     = extractor_->get_crc_len();
}

std::shared_ptr<AbstractSerializableMessage> TCPMessageExtractor::find_message()
{
    std::shared_ptr<AbstractSerializableMessage> msg;
    uint8_t* data;
    bool is_crc_ok = true;
    bool is_footer_ok = true;
    bool has_len = false;
    int data_len;
    for (auto section : packet_sections_) {
        switch(section) {
        case PacketSections::Header : {
            find_header();
            break;
        }
        case PacketSections::CMD : {
            std::string cmd = get_next_bytes(cmd_len_);
            msg = message_factory_->build_message(cmd.data());
            break;
        }
        case PacketSections::Length : {
            std::string len_size = get_next_bytes(data_len);
            data_len = calc_len(len_size.data(), packet_len_, is_pkt_len_msb_);

            has_len = true;
            break;
        }
        case PacketSections::Data : {
            if (has_len) {
                data = new uint8_t[data_len];
            } else {
                data = new uint8_t[msg->get_serialize_size()];
            }

            auto buf_ret = buffer_->read(data, data_len);
            if (buf_ret != BufferError::BUF_NOERROR) {
                std::cout << "fucking buffer" << std::endl;
            }
            break;
        }
        case PacketSections::CRC : {
            std::string crc_data = get_next_bytes(crc_len_);
            is_crc_ok = crc_checker_->is_valid((char*)data, data_len, crc_data.data(), crc_data.size());
            break;
        }
        case PacketSections::Footer : {
            is_footer_ok = can_find_footer();
            break;
        }
        case PacketSections::Other :{

            break;
        }
        }
    }
    if (is_crc_ok && is_footer_ok)
        msg->deserialize((char*)data, data_len);
    return msg;
}

void TCPMessageExtractor::find_header()
{
    int header_index = 0;
    while(1) {
        if (header_index == header_len_)
            break;
        if (header_content_[header_index] ==  buffer_->read_next_byte())
            header_index++;
        else
            header_index = 0;
    }
}


int TCPMessageExtractor::calc_len(const char * data, uint32_t size, bool is_msb)
{
    int len;
    switch (size) {
    case 1 : {
        len = (int)(*data);
        break;
    }
    case 2 : {
        if (is_msb)
            len = int((unsigned char)(data[0]) << 8 | (unsigned char)(data[1]));
        else
            len = int((unsigned char)(data[1]) << 8 | (unsigned char)(data[0]));
        break;
    }
    case 3 : {

    }
    case 4: {
        if (is_msb)
            len =  int((unsigned char)(data[0]) << 24 | (unsigned char)(data[1]) << 16 |  (unsigned char)(data[2]) << 8 | (unsigned char)(data[3]));
        else
            len =  int((unsigned char)(data[3]) << 24 | (unsigned char)(data[2]) << 16 |  (unsigned char)(data[1]) << 8 | (unsigned char)(data[0]));

    }
    case 5 : {

    }
    case 6 : {

    }
    case 7 : {

    }
    case 8 : {

    }
    }
}


bool TCPMessageExtractor::can_find_footer()
{
    std::string footer = get_next_bytes(footer_len_);
    for(int index = 0; index < footer_len_; index++) {
        if (footer[index] != footer_content_[index])
            return false;
    }
    return true;
}

std::string TCPMessageExtractor::get_next_bytes(uint32_t size)
{
    std::string data;
    data.resize(size);
    for (uint32_t i = 0; i < size; i++)
        data[i] = buffer_->read_next_byte();
    return data;
}

} // namespace peripheral
} // namespace hp

