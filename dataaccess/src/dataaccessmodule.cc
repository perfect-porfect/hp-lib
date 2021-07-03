#include "dataaccessmodule.h"
#include "messages/config.h"
#include <functional>
#include <iostream>

namespace perfect {
namespace dataaccess {

DataAccessModule::DataAccessModule(SharedCommunication module_communications)
    : module_communications_(module_communications)
{
    module_name_ = ModuleName::DataAccess;
    fs_manager_ = std::make_shared<fs::FSManager>();
}

void DataAccessModule::start()
{
    isRunning_ = true;
}

void DataAccessModule::stop()
{
    isRunning_ = false;
}

void DataAccessModule::initialize()
{
    module_communications_->add_connection(std::bind(&DataAccessModule::message_received, this, std::placeholders::_1, std::placeholders::_2), get_name());
}

bool DataAccessModule::is_running()
{
    return isRunning_;
}

ModuleName DataAccessModule::get_name() const
{
    return module_name_;
}

void DataAccessModule::message_received(Shared_MSG message, ModuleName src)
{
    auto msg_type = static_cast<messages::MessageType>(message->get_type());
    if (msg_type < messages::MessageType::DataBase) {
        std::cout << "Message for FileSystem" << std::endl;
        fs_manager_->message_handler(message);
    } else if (msg_type < messages::MessageType::LogManagment) {
        std::cout << "Message for Database" << std::endl;
    } else {
        std::cout << "Message for Log Managment" << std::endl;
    }
}

void DataAccessModule::sendMessage(Shared_MSG msg, ModuleName dest)
{
   module_communications_->send_message(msg, module_name_, dest);
}

} // dataaccess
} // perfect
