#pragma once

#include "interface_file_system.h"
#include "messages/config.h"
#include <map>
#include <bits/shared_ptr.h>
#include "fs/file_manager.h"

namespace perfect {
namespace dataaccess {
namespace fs {

class FSManager : std::enable_shared_from_this<FSManager> ,public InterfaceFileSystemManager  {
public:
    FSManager();
    void message_handler(Shared_MSG msg);
    virtual std::shared_ptr<InterfaceDirectoryManager> get_directory_manager() const;
    virtual std::shared_ptr<InterfaceFileManager> get_file_manager() const;
    virtual void add_connection(messages::MessageType msg_type, MessageFunctionCallback callback);
    virtual ~FSManager();
private:
    std::shared_ptr<InterfaceFileManager> file_manager_;
    std::shared_ptr<InterfaceDirectoryManager> directory_manager_;
    std::map<messages::MessageType, MessageFunctionCallback> messages_func_map_;
//    std::shared_ptr<FSManager> shared_this_;
};

//class MessageHandler {
//public:
//    explicit MessageHandler(messages::MessageType msg_type, MessageFunctionCallback func){ msg_type_ = msg_type; func_ = func;}
////    virtual void message_handler(Shared_MSG msg) = 0;
//    void init_my_sef() { manager_.add_connection(msg_type_, func_); }
//private:
//    MessageFunctionCallback func_;
//    FSManager manager_;
//    messages::MessageType msg_type_;
//};



} // fs
} // dataaccess
} // perfect
