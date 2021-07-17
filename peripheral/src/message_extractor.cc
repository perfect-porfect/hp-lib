#include "message_extractor.h"
#include <iostream>

namespace hp {
namespace peripheral {

MessageExtractor::MessageExtractor(AbstractPacketSections *extractor, AbstractBuffer *buffer)
    : extractor_(extractor), buffer_(buffer)
{
    is_header_exist_ = false;
    is_cmd_exist_    = false;
    is_length_exist_ = false;
    is_data_exist_   = false;
    is_crc_exist_    = false;
    is_footer_exist_ = false;

    crc_include_header_ = false;
    crc_include_footer_ = false;
    crc_include_length_ = false;
    crc_include_data_ = false;
    crc_include_cmd_ = false;
    crc_include_crc_ = false;


    header_size_ = 0;
    footer_size_ = 0;
    data_size_   = 0;
    cmd_size_    = 0;
    len_size_    = 0;
    crc_size_    = 0;

    packet_sections_ = extractor_->get_packet_sections();
    for (auto section : packet_sections_) {
        auto type = section->get_type();
        switch(type) {
        case PacketSections::Header : {
            header_ = dynamic_cast<HeaderSection*>(section);
            header_size_ = header_->content.size();
            is_header_exist_ = true;
            break;
        }
        case PacketSections::CMD : {
            cmd_ = dynamic_cast<CMDSection*>(section);
            cmd_size_ = cmd_->size_bytes;
            is_cmd_exist_ = true;
            break;
        }
        case PacketSections::Length : {
            length_ = dynamic_cast<LengthSection*>(section);
            len_size_ = length_->size_bytes;
            is_length_exist_ = true;
            break;
        }
        case PacketSections::Data : {
            data_ = dynamic_cast<DataSection*>(section);
            data_size_ = data_->fix_size_bytes;
            is_data_exist_ = true;
            break;
        }
        case PacketSections::CRC : {
            crc_ = dynamic_cast<CRCSection*>(section);
            crc_size_ = crc_->size_bytes;
            is_crc_exist_ = true;
            if (crc_->include & PacketSections::Data)
                crc_include_data_ = true;
            if (crc_->include & PacketSections::CMD)
                crc_include_cmd_ = true;
            if (crc_->include & PacketSections::Length)
                crc_include_length_ = true;
            if (crc_->include & PacketSections::Footer)
                crc_include_footer_ = true;
            if (crc_->include & PacketSections::Header)
                crc_include_header_ = true;
            if (crc_->include & PacketSections::CRC)
                crc_include_crc_ = true;
            break;
        }
        case PacketSections::Footer : {
            footer_ = dynamic_cast<FooterSection*>(section);
            footer_size_ = footer_->content.size();
            is_footer_exist_ = true;
            break;
        }
        case PacketSections::Other :{

            break;
        }
        }

    }

    // check failed packet
    {
        // if crc include's cmd but packet didn't
        // if len include somthing but packet didn't
        // warning for crc not include data
        // warning if length not include data
    }
}

std::shared_ptr<AbstractSerializableMessage> MessageExtractor::find_message()
{
    std::shared_ptr<AbstractSerializableMessage> msg = nullptr;
    bool is_find_message = false;
    while (!is_find_message) {
        std::map<PacketSections, std::string> crc_check_data;
        std::map<PacketSections, std::string> packet_sections;
        std::string footer;
        std::string packet;
        std::string data;
        std::string cmd;
        std::string len;
        std::string crc;
        bool is_footer_ok = true;
        bool is_cmd_null = false;
        bool is_crc_ok = true;
        uint32_t data_len = 0;
        uint32_t data_size = 0;
        for (auto section : packet_sections_) {
            if (is_cmd_null || !is_footer_ok)
                break;
            auto type = section->get_type();
            switch(type) {
            case PacketSections::Header : {
                find_header();
                break;
            }
            case PacketSections::CMD : {
                cmd = get_next_bytes(cmd_->size_bytes);
                msg = cmd_->msg_factory->build_message(cmd.data());
                if (msg == nullptr) {
                    packet_sections[PacketSections::Header] = header_->content;
                    packet_sections[PacketSections::CMD] = cmd;
                    extractor_->get_error_packet(PacketErrors::Wrong_CMD, packet_sections);
                    is_cmd_null = true;
                    continue;
                }
                is_cmd_exist_ = true;
                break;
            }
            case PacketSections::Length : {
                len = get_next_bytes(length_->size_bytes);
                uint32_t len_val = calc_len(len.data(), length_->size_bytes, length_->is_first_byte_msb);
                if (length_->include != PacketSections::Data) {
                    if (length_->include & PacketSections::CMD)
                        len_val -= cmd_size_;
                    if (length_->include & PacketSections::Length)
                        len_val -= len_size_;
                    if (length_->include & PacketSections::Footer)
                        len_val -= footer_size_;
                    if (length_->include & PacketSections::Header)
                        len_val -= header_size_;
                    if (length_->include & PacketSections::CRC)
                        len_val -= crc_size_;
                }
                data_len = len_val;
                is_length_exist_ = true;
                break;
            }
            case PacketSections::Data : {
                if (is_length_exist_) {
                    data.resize(data_len);
                    data_size = data_len;
                } else {
                    if (msg != nullptr)
                        data_size = msg->get_serialize_size();
                    else
                        data_size = data_->fix_size_bytes;
                    data.resize(data_size);
                }
                buffer_->read((uint8_t*)data.data(), data_size);
                break;
            }
            case PacketSections::CRC : {
                crc = get_next_bytes(crc_->size_bytes);
                if (crc_->include != PacketSections::Data) {
                    if (crc_->include & PacketSections::CMD)
                        crc_ -= cmd_size_;
                    if (length_->include & PacketSections::Length)
                        crc_ -= len_size_;
                    if (length_->include & PacketSections::Footer)
                        crc_ -= footer_size_;
                    if (length_->include & PacketSections::Header)
                        crc_ -= header_size_;
                    if (length_->include & PacketSections::CRC)
                        crc_ -= crc_size_;
                }

                break;
            }
            case PacketSections::Footer : {
                footer = get_next_bytes(footer_->content.size());
                if (footer.compare(footer_->content) != 0) {
                    is_footer_ok = false;
                    packet_sections[PacketSections::Data] = data;
                    packet_sections[PacketSections::CMD] = cmd;
                    packet_sections[PacketSections::Footer] = footer;
                    packet_sections[PacketSections::Header] = data;
                    packet_sections[PacketSections::CRC] = crc;
                    packet_sections[PacketSections::Length] = len;
                    extractor_->get_error_packet(PacketErrors::Wrong_Footer, packet_sections);
                    continue;
                }
                break;
            }
            case PacketSections::Other :{

                break;
            }
            }
        }
        if (is_crc_exist_) {
            if (crc_include_data_)
                packet_sections[PacketSections::Data] = data;
            if (crc_include_cmd_)
                packet_sections[PacketSections::CMD] = cmd;
            if (crc_include_footer_)
                packet_sections[PacketSections::Footer] = footer;
            if (crc_include_header_)
                packet_sections[PacketSections::Header] = data;
            if (crc_include_crc_)
                packet_sections[PacketSections::CRC] = crc;
            if (crc_include_length_)
                packet_sections[PacketSections::Length] = len;
            is_crc_ok = crc_->crc_checker->is_valid(packet_sections, crc);
            if (!is_crc_ok) {
                extractor_->get_error_packet(PacketErrors::Wrong_CRC, packet_sections);
                continue;
            }
        }
        if (is_crc_ok && is_footer_ok && msg != nullptr) {
            msg->deserialize((char*)data.data(), data_size);
            is_find_message = true;
        }
    }
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
        else {
            header_index = 0;
        }
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


std::string MessageExtractor::get_next_bytes(uint32_t size)
{
    std::string data;
    data.resize(size);
    for (uint32_t i = 0; i < size; i++)
        data[i] = buffer_->read_next_byte();
    return data;
}

template<class Containter>
void MessageExtractor::fill_packet(std::string& source, const Containter& data)
{
    source.resize(source.size() + data.size());
    std::copy(data.begin(), data.end(), source.end() - data.size());
}

} // namespace peripheral
} // namespace hp
