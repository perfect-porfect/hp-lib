#include "fs/fs_manager.h"
#include "perfect-lib/dataaccess/messages/config.h"
namespace perfect {
namespace dataaccess {
namespace fs {

FSManager::FSManager()
{
    file_manager_ = std::make_shared<FileManager>();
    add_connection(messages::MessageType::READ_FILE, std::bind(&InterfaceFileManager::read_file, file_manager_, std::placeholders::_1));
    add_connection(messages::MessageType::WRITE_FILE, std::bind(&InterfaceFileManager::write_file, file_manager_, std::placeholders::_1));
}

void FSManager::message_handler(Shared_MSG msg)
{
    auto msg_type = static_cast<messages::MessageType>(msg->get_type());
    auto func = messages_func_map_[msg_type];
    func(msg);
}

void FSManager::add_connection(messages::MessageType msg_type, MessageFunctionCallback callback)
{
    messages_func_map_[msg_type] = callback;
}

std::shared_ptr<InterfaceDirectoryManager> FSManager::get_directory_manager() const
{
    return directory_manager_;
}

std::shared_ptr<InterfaceFileManager> FSManager::get_file_manager() const
{
    return file_manager_;
}

FSManager::~FSManager()
{

}

} // fs
} // dataaccess
} // perfect

