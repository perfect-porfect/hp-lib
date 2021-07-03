#pragma once

#include <memory>
#include <functional>

#include "interface_message.h"

namespace perfect {

enum class ModuleName{
    DataAccess,
    SignalProcess,
    DataProcess,
    GUI,
    Communication,
    Controller
};
typedef std::shared_ptr<common::InterfaceMessage> Shared_MSG;
typedef std::function<void (Shared_MSG msg, ModuleName src) > CommunicationFunctionCallback;
typedef std::function<void (Shared_MSG msg) > MessageFunctionCallback;

} // perfect
