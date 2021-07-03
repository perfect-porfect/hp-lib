#pragma once

#include <functional>
#include <boost/signals2.hpp>

#include <perfect-lib/common/interface_message.h>
#include <perfect-lib/common/interface_module.h>
#include <perfect-lib/common/config.h>

namespace perfect {
namespace communication {
enum class CommunicationError{
    Ok
};

//!
//! \brief InterfaceModuleCommunication class is an interface which is used for send msgs between
//! all modules. You can send your msgs with sync & async.
//! becarefull about the threads that create in async function
//!

class InterfaceCommunication{
public:
    virtual CommunicationError add_connection(CommunicationFunctionCallback func, ModuleName moduleName) = 0;
    virtual CommunicationError remove_connection(ModuleName destination) = 0;
    virtual CommunicationError send_message(Shared_MSG msg, ModuleName src, ModuleName dst) = 0;
    virtual ~InterfaceCommunication() {}
};

//!
//! \brief InterfaceCommunicationThread class is an interface which is used for send msgs between
//! all modules. You can send your msgs with sync & async.
//! becarefull about the threads that create in async function
//!
class InterfaceCommunicationThread : public InterfaceCommunication {
public:
    virtual CommunicationError send_message_async(Shared_MSG msg, ModuleName src, ModuleName dst) = 0;
    virtual int number_of_thread_executed() const = 0;
    virtual ~InterfaceCommunicationThread() {}
};

class InterfaceModuleCommunication : public InterfaceCommunication, public common::InterfaceModule {
public:
    virtual ~InterfaceModuleCommunication() {} //! interface virtual destructor
};

}  // communication
typedef std::shared_ptr<communication::InterfaceModuleCommunication> SharedCommunication;
}  // perfect
