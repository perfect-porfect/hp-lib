#pragma once

#include <memory>

#include "perfect-lib/common/config.h"
#include "messages/config.h"

namespace perfect{
namespace dataaccess{
namespace fs {

class InterfaceFileManager{
public:
    virtual void write_file(Shared_MSG msg) = 0;
    virtual void read_file(Shared_MSG msg) = 0;
    virtual ~InterfaceFileManager() {}
};

class InterfaceDirectoryManager{
public:
    virtual ~InterfaceDirectoryManager() {}
};

class InterfaceFileSystemManager{
public:
    virtual std::shared_ptr<InterfaceDirectoryManager> get_directory_manager() const = 0;
    virtual std::shared_ptr<InterfaceFileManager>      get_file_manager() const = 0;
    virtual void add_connection(messages::MessageType msg_type, MessageFunctionCallback callback) = 0;
    virtual void message_handler(Shared_MSG msg_type) = 0;
};


} // fs
} // dataaccess
} // perfect
