#pragma once

#include "perfect-lib/common/interface_module.h"
#include "perfect-lib/communication/interface_module_communication.h"
namespace perfect {
namespace controller{


class ControllerModule : public common::InterfaceModule {
public:
    ControllerModule(SharedCommunication module_communication);
    void start();
    void stop();
    bool is_running();
    void initialize();
    ModuleName get_name() const;
private:
    ModuleName module_name_;
    bool is_running_;
    SharedCommunication module_communication_;
};


}
}
