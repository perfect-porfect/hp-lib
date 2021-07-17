#pragma once

#include <string>
#include "config.h"

namespace perfect{
namespace common{

class InterfaceModule {
public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool is_running() = 0;
    virtual void initialize() = 0;
    virtual ModuleName get_name() const = 0;
    virtual ~InterfaceModule(){} //! interface virtual destructor
};

}
}
