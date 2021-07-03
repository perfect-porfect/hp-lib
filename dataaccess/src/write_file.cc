#include "messages/write_file.h"

namespace perfect {
namespace dataaccess {
namespace messages {


WriteFileMessage::WriteFileMessage(std::string file_address, std::string content, int id)
    : file_address_(file_address), content_file_(content), id_(id)
{

}

int WriteFileMessage::get_type() const
{
    return static_cast<int>(MessageType::WRITE_FILE);
}

int WriteFileMessage::get_id() const
{
    return id_;
}

std::string WriteFileMessage::get_content() const
{
    return content_file_;
}

std::string WriteFileMessage::get_file_address() const
{
    return file_address_;
}

WriteFileMessage::~WriteFileMessage()
{

}

} // messages
} // dataaccess
} // perfect
