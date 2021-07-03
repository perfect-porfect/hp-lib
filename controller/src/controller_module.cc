#include "controller_module.h"

namespace perfect {
namespace controller{

ControllerModule::ControllerModule(SharedCommunication module_communication) : module_communication_(module_communication)
{
    module_name_ = ModuleName::Controller;
    is_running_ = false;
}

void ControllerModule::start()
{

}

void ControllerModule::stop()
{

}

bool ControllerModule::is_running()
{
    return is_running_;
}

void ControllerModule::initialize()
{

}

ModuleName ControllerModule::get_name() const
{
    return module_name_;
}

} // controller
} // perfect
