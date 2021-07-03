#ifndef TCPMESSAGEEXTRACTOR_H
#define TCPMESSAGEEXTRACTOR_H

#include "abstract_peripheral.h"

namespace hp {
namespace peripheral {

class TCPMessageExtractor {
public:
    TCPMessageExtractor(std::shared_ptr<AbstractRawExtractor> extractor, std::shared_ptr<AbstractBuffer> buffer);
    std::shared_ptr<AbstractSerializableMessage> find_message();
private:
    void find_header();
    bool can_find_footer();
    int calc_len(const char *data, uint32_t size, bool is_msb);
    std::string get_next_bytes(uint32_t size);

    //Extractor
    std::shared_ptr<AbstractMessageFactory> message_factory_;
    std::shared_ptr<AbstractRawExtractor> extractor_;
    std::shared_ptr<AbstractCRC> crc_checker_;
    std::shared_ptr<AbstractBuffer> buffer_;
    int footer_len_;
    int header_len_;
    int packet_len_;
    int cmd_len_;
    int crc_len_;
    bool is_pkt_len_msb_;
    PacketSections pkt_len_include_;
    std::string header_content_;
    std::string footer_content_;
    std::vector<PacketSections> packet_sections_;
};

} // namespace peripheral
} // namespace hp

#endif // TCPMESSAGEEXTRACTOR_H
