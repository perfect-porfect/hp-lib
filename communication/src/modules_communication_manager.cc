#include "modules_communication_manager.h"
#include <iostream>

namespace perfect {
namespace communication {

ModulesCommunicationManager::ModulesCommunicationManager()
{
    module_name_ = ModuleName::Communication;
}

void ModulesCommunicationManager::start()
{

}

void ModulesCommunicationManager::stop()
{

}

bool ModulesCommunicationManager::is_running()
{
    return true;
}

void ModulesCommunicationManager::initialize()
{

}

ModuleName ModulesCommunicationManager::get_name() const
{
    return module_name_;
}

CommunicationError ModulesCommunicationManager::add_connection(CommunicationFunctionCallback func, ModuleName module_name)
{
    std::shared_lock<std::shared_mutex> lock(connections_lock_);
    module_maps_[module_name] = func;
    return CommunicationError::Ok;
}

CommunicationError ModulesCommunicationManager::send_message(Shared_MSG msg, ModuleName src, ModuleName dst)
{
    CommunicationFunctionCallback func = module_maps_[dst];
    func(msg, src);
    return CommunicationError::Ok;
}

CommunicationError ModulesCommunicationManager::send_message_async(Shared_MSG msg, ModuleName src, ModuleName dst)
{
    //This pointer hanlde by boost_group thread, you must not delete this object
    boost::thread* current_thread = new boost::thread(&InterfaceModuleCommunication::send_message, this, msg, src, dst);
    messages_thread_.add_thread(current_thread);
    return CommunicationError::Ok;
}

CommunicationError ModulesCommunicationManager::remove_connection(ModuleName dst)
{
    std::shared_lock<std::shared_mutex> lock(connections_lock_);
    module_maps_.erase(dst);
    return CommunicationError::Ok;
}

uint ModulesCommunicationManager::number_of_thread_executed()
{
    uint counter = 0;
    return counter;
}

void ModulesCommunicationManager::send_thread(Shared_MSG msg, ModuleName destination)
{
    CommunicationFunctionCallback func = module_maps_[destination];
    func(msg, destination);
}

ModulesCommunicationManager::~ModulesCommunicationManager()
{
    messages_thread_.join_all();
}

}
}
