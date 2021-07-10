#ifndef TCPMESSAGEEXTRACTOR_H
#define TCPMESSAGEEXTRACTOR_H

#include "abstract_peripheral.h"

namespace hp {
namespace peripheral {

class MessageExtractor {
public:
    MessageExtractor(std::shared_ptr<AbstractRawExtractor> extractor, std::shared_ptr<AbstractBuffer> buffer);
    std::shared_ptr<AbstractSerializableMessage> find_message();
private:
    void find_header();
    bool can_find_footer();
    int calc_len(const char *data, uint32_t size, bool is_msb);
    std::string get_next_bytes(uint32_t size);

    //Extractor
    std::shared_ptr<AbstractRawExtractor> extractor_;
    std::shared_ptr<AbstractBuffer> buffer_;
    std::vector<Section*> packet_sections_;

    HeaderSection* header_;
    CMDSection* cmd_;
    LengthSection* length_;
    CRCSection* crc_;
    DataSection* data_;
    FooterSection* footer_;


};

} // namespace peripheral
} // namespace hp

#endif // TCPMESSAGEEXTRACTOR_H
