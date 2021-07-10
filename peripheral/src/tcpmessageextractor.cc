#include "tcpmessageextractor.h"
#include <iostream>

namespace hp {
namespace peripheral {

MessageExtractor::MessageExtractor(std::shared_ptr<AbstractPacketSections> extractor, std::shared_ptr<AbstractBuffer> buffer)
    : extractor_(extractor), buffer_(buffer)
{
    packet_sections_ = extractor_->get_packet_sections();
}

std::shared_ptr<AbstractSerializableMessage> MessageExtractor::find_message()
{
    std::shared_ptr<AbstractSerializableMessage> msg;
//    uint8_t* data = nullptr;
    std::vector<uint8_t> data;
    bool is_crc_ok = true;
    bool is_footer_ok = true;
    bool has_len = false;
    int data_len;
    uint32_t data_size = 0;
    for (auto section : packet_sections_) {
        auto type = section->get_type();
        switch(type) {
        case PacketSections::Header : {
            header_ = dynamic_cast<HeaderSection*>(section);
            find_header();
            std::cout << "find header" << std::endl;
            break;
        }
        case PacketSections::CMD : {
            cmd_ = dynamic_cast<CMDSection*>(section);
            std::string cmd = get_next_bytes(cmd_->size_bytes);
            msg = cmd_->msg_factory->build_message(cmd.data());
            break;
        }
        case PacketSections::Length : {
            length_ = dynamic_cast<LengthSection*>(section);
            std::string len_size = get_next_bytes(length_->size_bytes);
            data_len = calc_len(len_size.data(), length_->size_bytes, length_->is_msb);
            has_len = true;
            break;
        }
        case PacketSections::Data : {
            if (has_len) {
                data.resize(data_len);
//                data = new uint8_t[data_len];
                data_size = data_len;
            } else {
                data_size = msg->get_serialize_size();
//                data = new uint8_t[data_size];
                data.resize(data_size);
            }

            auto buf_ret = buffer_->read(data.data(), data_size);
            if (buf_ret != BufferError::BUF_NOERROR) {
                std::cout << "fucking buffer" << std::endl;
            }
            break;
        }
        case PacketSections::CRC : {
            crc_ = dynamic_cast<CRCSection*>(section);
            std::string crc_data = get_next_bytes(crc_->size_bytes);
            is_crc_ok = crc_->crc_checker->is_valid((char*)data.data(), data_size, crc_data.data(), crc_data.size());
            break;
        }
        case PacketSections::Footer : {
            footer_ = dynamic_cast<FooterSection*>(section);
            is_footer_ok = can_find_footer();
            break;
        }
        case PacketSections::Other :{

            break;
        }
        }
    }
    if (is_crc_ok && is_footer_ok && msg != nullptr)
        msg->deserialize((char*)data.data(), data_size);
    return msg;
}

void MessageExtractor::find_header()
{
    uint32_t header_index = 0;
    while(1) {
        if (header_index == header_->content.size())
            break;
        char header = buffer_->read_next_byte();
        if (header_->content[header_index] ==  header)
            header_index++;
        else
            header_index = 0;
    }
}


int MessageExtractor::calc_len(const char * data, uint32_t size, bool is_msb)
{
    int len = 0;
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
    return len;
}


bool MessageExtractor::can_find_footer()
{
    for (uint32_t index = 0; index < footer_->content.size(); index++)
        //TODO(HP) if bytes not valid store it and find to header.
        if (footer_->content[index] ==  buffer_->read_next_byte())
            return false;
    return true;

}

std::string MessageExtractor::get_next_bytes(uint32_t size)
{
    std::string data;
    data.resize(size);
    for (uint32_t i = 0; i < size; i++)
        data[i] = buffer_->read_next_byte();
    return data;
}

} // namespace peripheral
} // namespace hp

