#pragma once

#include <messages/config.h>
#include <perfect-lib/common/interface_message.h>

namespace perfect {
namespace dataaccess {
namespace messages {

class WriteFileMessage : public common::InterfaceMessage {
public:
    WriteFileMessage(std::string file_address, std::string content, int id);
    std::string get_content() const;
    std::string get_file_address() const;
    // InterfaceMessage
    virtual int get_type() const final;
    virtual int get_id() const final ;
    virtual ~WriteFileMessage();
private:
    std::string file_address_;
    std::string content_file_;
    int id_;
};

} // messages
} // dataaccess
} // perfect
