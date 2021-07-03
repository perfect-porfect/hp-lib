#pragma once

#include "interface_file_system.h"

namespace perfect {
namespace dataaccess {
namespace fs {


class FileManager : public InterfaceFileManager {
public:
    FileManager();
    virtual void write_file(Shared_MSG msg);
    virtual void read_file(Shared_MSG msg);
private:
    void write(const std::string &file_address, const std::string &content);
    std::string read(const std::string &file_address);
    InterfaceFileSystemManager* fs_manager_;
};

} // fs
} // dataaccess
} // perfect
