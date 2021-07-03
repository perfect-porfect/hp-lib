#include "fs/file_manager.h"
#include "messages/write_file.h"
#include "messages/read_file.h"
#include <fstream>
#include <chrono>
#include <iostream>

namespace perfect {
namespace dataaccess {
namespace fs {

FileManager::FileManager()
{

}

void FileManager::write_file(Shared_MSG msg)
{
    auto write_msg = std::dynamic_pointer_cast<messages::WriteFileMessage>(msg);
    write(write_msg->get_file_address(), write_msg->get_content());
}

void FileManager::read_file(Shared_MSG msg)
{
    auto read_msg = std::dynamic_pointer_cast<messages::ReadFileMessage>(msg);
    std::string content = read(read_msg->get_file_address());
    read_msg->set_content(content);
}

void FileManager::write(const std::string &file_address, const std::string &content)
{
    std::ofstream myfile;
    myfile.open(file_address);
    myfile << content;
    myfile.close();
}

std::string FileManager::read(const std::string &file_address)
{
    std::string line;
    std::string content = "";
    std::ifstream myfile(file_address);
    if (myfile.is_open()) {
        while (getline (myfile, line))
            content += line + "\n";
        myfile.close();
    }
    return content;
}

} // fs
} // dataaccess
} // perfect
