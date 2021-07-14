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

    packet_sections_ = extractor_->get_packet_sections();
    for (auto section : packet_sections_) {
        auto type = section->get_type();
        switch(type) {
        case PacketSections::Header : {
            header_ = dynamic_cast<HeaderSection*>(section);
            is_header_exist_ = true;
            break;
        }
        case PacketSections::CMD : {
            cmd_ = dynamic_cast<CMDSection*>(section);
            is_cmd_exist_ = true;
            break;
        }
        case PacketSections::Length : {
            length_ = dynamic_cast<LengthSection*>(section);
            is_length_exist_ = true;
            break;
        }
        case PacketSections::Data : {
            data_ = dynamic_cast<DataSection*>(section);
            is_data_exist_ = true;
            break;
        }
        case PacketSections::CRC : {
            crc_ = dynamic_cast<CRCSection*>(section);
            is_crc_exist_ = true;
            break;
        }
        case PacketSections::Footer : {
            footer_ = dynamic_cast<FooterSection*>(section);
            is_footer_exist_ = true;
            break;
        }
        case PacketSections::Other :{

            break;
        }
        }

    }
}



std::shared_ptr<AbstractSerializableMessage> MessageExtractor::find_message()
{
    std::shared_ptr<AbstractSerializableMessage> msg = nullptr;
    std::vector<uint8_t> data;
    bool is_find_message = false;
    while(!is_find_message) {
        std::string packet;
        bool is_crc_ok = true;
        bool is_footer_ok = true;
        int data_len;
        uint32_t data_size = 0;
        for (auto section : packet_sections_) {
            auto type = section->get_type();
            switch(type) {
            case PacketSections::Header : {
                find_header();
                fill_packet(packet, header_->content);
                std::copy(header_->content.begin(), header_->content.end(), packet.begin());
                break;
            }
            case PacketSections::CMD : {
                std::string cmd = get_next_bytes(cmd_->size_bytes);
                msg = cmd_->msg_factory->build_message(cmd.data());
                fill_packet(packet, cmd);
                if (msg == nullptr)
                    extractor_->get_error_packet(PacketErrors::Wrong_CMD, packet.data(), packet.size());
                is_cmd_exist_ = true;
                break;
            }
            case PacketSections::Length : {
                std::string len_size = get_next_bytes(length_->size_bytes);
                data_len = calc_len(len_size.data(), length_->size_bytes, length_->is_first_byte_msb);
                is_length_exist_ = true;
                fill_packet(packet, len_size);
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
                auto buf_ret = buffer_->read(data.data(), data_size);
                fill_packet(packet, data);
                if (buf_ret != BufferError::BUF_NOERROR) {
                    std::cout << "Fucking buffer" << std::endl;
                }
                break;
            }
            case PacketSections::CRC : {
                std::string crc_data = get_next_bytes(crc_->size_bytes);
                is_crc_ok = crc_->crc_checker->is_valid((char*)data.data(), data_size, crc_data.data(), crc_data.size());
                fill_packet(packet, crc_data);
                break;
            }
            case PacketSections::Footer : {
                std::string footer = get_next_bytes(footer_->content.size());
                if (footer.compare(footer_->content) != 0)
                    is_footer_ok = false;
                fill_packet(packet, footer);
                break;
            }
            case PacketSections::Other :{

                break;
            }
            }
        }
        if (!is_crc_ok) {
            extractor_->get_error_packet(PacketErrors::Wrong_CRC, packet.data(), packet.size());
            continue;
        } else if (!is_footer_ok) {
            extractor_->get_error_packet(PacketErrors::Wrong_Footer, packet.data(), packet.size());
            continue;
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
