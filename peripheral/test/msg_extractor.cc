#include "message_extractor.h"
#include "hp/common/buffer/circular_buffer.h"
#include <iostream>
#include <vector>
#include <thread>


#define HEADER1         (0xaa)
#define HEADER2         (0xff)
#define CMD_SIZE        (2)
#define LENGTH_SIZE     (2)
#define CRC_SIZE        (2)
#define FOOTER1         (0xbb)
#define FOOTER2         (0xcc)

using namespace hp::peripheral;


class Include_Packet_Wrong : public AbstractPacketStructure {
public:
    void init_packet_structure(std::vector<std::shared_ptr<Section>> &packet_section)
    {
        auto length = std::make_shared<LengthSection>();
        length->include = PacketSections::Header | PacketSections::CMD | PacketSections::Footer;
        length->is_first_byte_msb = true;
        length->size_bytes = LENGTH_SIZE;

        auto data = std::make_shared<DataSection>();
        auto crc = std::make_shared<CRCSection>();
        crc->size_bytes = CRC_SIZE;
        crc->include = PacketSections::Header | PacketSections::CMD | PacketSections::Footer;

        auto footer = std::make_shared<FooterSection>();
        footer->content = std::string{static_cast<char>FOOTER1, static_cast<char>FOOTER2};

        packet_section.push_back(length);
        packet_section.push_back(data);
        packet_section.push_back(crc);
        packet_section.push_back(footer);
    };

    void packet_section_error(PacketErrors error, const std::map<PacketSections, std::string> &packet) {

    }
};


class PacketSection : public AbstractPacketStructure {
public:
    void init_packet_structure(std::vector<Section *> &packet_section)
    {
        HeaderSection* header = new HeaderSection();
        header->content = std::string{static_cast<char>HEADER1, static_cast<char>HEADER2};

        CMDSection* cmd = new CMDSection();
        cmd->size_bytes = CMD_SIZE;

        LengthSection* length = new LengthSection();
        length->include = PacketSections::Data;
        length->is_first_byte_msb = true;
        length->size_bytes = LENGTH_SIZE;

        DataSection* data = new DataSection();
        CRCSection* crc = new CRCSection();
        crc->size_bytes = CRC_SIZE;

        FooterSection* footer = new FooterSection();
        footer->content = std::string{static_cast<char>FOOTER1, static_cast<char>FOOTER2};
        packet_section.push_back(header);
        packet_section.push_back(cmd);
        packet_section.push_back(length);
        packet_section.push_back(data);
        packet_section.push_back(crc);
        packet_section.push_back(footer);
    };

    void packet_section_error(PacketErrors error, const std::map<PacketSections, std::string>& packet){

    }
};

int main()
{
        auto packet = std::make_shared<Include_Packet_Wrong>();
        auto buffer = std::make_shared<CircularBuffer>();
        MessageExtractor extractor(packet, buffer);

    //    std::shared_ptr<CircularBuffer> buffer_(buffer);
    //    delete packet;
    //    delete buffer;
}
