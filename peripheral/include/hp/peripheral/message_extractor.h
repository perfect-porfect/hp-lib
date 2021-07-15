#ifndef TCPMESSAGEEXTRACTOR_H
#define TCPMESSAGEEXTRACTOR_H

#include "abstract_peripheral.h"

namespace hp {
namespace peripheral {

class MessageExtractor {
public:
    MessageExtractor(AbstractPacketSections* extractor, AbstractBuffer* buffer);
    std::shared_ptr<AbstractSerializableMessage> find_message();
private:
    void find_header();
    int calc_len(const char *data, uint32_t size, bool is_msb);
    std::string get_next_bytes(uint32_t size);
    template<class Containter>
    void fill_packet(std::string& source, const Containter &data);

    //Extractor
    AbstractPacketSections* extractor_;
    AbstractBuffer* buffer_;
    std::vector<Section*> packet_sections_;

    const HeaderSection* header_;
    const CMDSection* cmd_;
    const LengthSection* length_;
    const CRCSection* crc_;
    const DataSection* data_;
    const FooterSection* footer_;

    bool is_header_exist_;
    bool is_cmd_exist_;
    bool is_length_exist_;
    bool is_data_exist_;
    bool is_crc_exist_;
    bool is_footer_exist_;

    uint32_t cmd_size_;
    uint32_t header_size_;
    uint32_t len_size_;
    uint32_t data_size_;
    uint32_t crc_size_;
    uint32_t footer_size_;

};

} // namespace peripheral
} // namespace hp

#endif // TCPMESSAGEEXTRACTOR_H
