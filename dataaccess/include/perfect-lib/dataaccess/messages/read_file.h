#pragma once

#include <perfect-lib/common/interface_message.h>

#include <messages/config.h>

namespace perfect {
namespace dataaccess {
namespace messages {

class ReadFileMessage : public common::InterfaceMessage {
public:
    ReadFileMessage(std::string file_address, int id);
    std::string get_content() const;
    void set_content(std::string content);
    std::string get_file_address() const;
    // InterfaceMessage
    int get_type() const;
    int get_id() const;
private:
    std::string file_address_;
    std::string content_file_;
    int id_;
};

} // messages
} // dataaccess
} // perfect
