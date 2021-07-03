#pragma once

#include <functional>
#include <map>
#include <boost/thread.hpp>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include "interface_module_communication.h"

#define COUNT_THREAD (10)

namespace perfect{
namespace communication {

class ModulesCommunicationManager : public InterfaceModuleCommunication
{
public:
    ModulesCommunicationManager();
    virtual void start() override;
    virtual void stop() override;
    virtual bool is_running() override;
    virtual void initialize() override;
    virtual ModuleName get_name() const override;
    virtual CommunicationError add_connection(CommunicationFunctionCallback func, ModuleName module_name) override;
    virtual CommunicationError send_message(Shared_MSG msg, ModuleName src, ModuleName dst) override;
    virtual CommunicationError send_message_async(Shared_MSG msg, ModuleName src, ModuleName dst);
    virtual CommunicationError remove_connection(ModuleName dst) override;
    virtual ~ModulesCommunicationManager() override;
    uint number_of_thread_executed();
private:
    void send_thread(Shared_MSG msg, ModuleName destination);
    std::map<ModuleName, CommunicationFunctionCallback> module_maps_; // pair of module name and function for connection
    boost::thread_group messages_thread_;
    std::shared_mutex connections_lock_;
    ModuleName module_name_;
};

} // communication
} // perfect
