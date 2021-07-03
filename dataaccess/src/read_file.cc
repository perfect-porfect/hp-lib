#include "messages/read_file.h"

namespace perfect {
namespace dataaccess {
namespace messages {


ReadFileMessage::ReadFileMessage(std::string file_address, int id)
    : file_address_(file_address), id_(id)
{

}

int ReadFileMessage::get_type() const
{
    return static_cast<int>(MessageType::READ_FILE);
}

int ReadFileMessage::get_id() const
{
    return id_;
}

std::string ReadFileMessage::get_content() const
{
    return content_file_;
}

void ReadFileMessage::set_content(std::string content)
{
    content_file_ = content;
}

std::string ReadFileMessage::get_file_address() const
{
    return file_address_;
}


} // messages
} // dataaccess
} // perfect
