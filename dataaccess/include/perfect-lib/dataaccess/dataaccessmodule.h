#pragma once

#include <memory>
#include "perfect-lib/common/interface_module.h"
#include "perfect-lib/communication/interface_module_communication.h"
#include "perfect-lib/common/config.h"
#include "fs/fs_manager.h"

namespace perfect{
namespace dataaccess{

class DataAccessModule : public common::InterfaceModule
{
public:
    DataAccessModule(SharedCommunication module_communications);
    void start();
    void stop();
    void initialize();
    bool is_running();
    ModuleName get_name() const;

private:
    ModuleName module_name_;
    bool isRunning_;
    SharedCommunication module_communications_;
    void message_received(Shared_MSG message, ModuleName src);
    void sendMessage(Shared_MSG msg, ModuleName dest);
    std::shared_ptr<fs::InterfaceFileSystemManager> fs_manager_;

    // InterfaceModule interface
public:
};

} // dataaccess
} // perfect
