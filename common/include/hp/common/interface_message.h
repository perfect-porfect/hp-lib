#pragma once

#include <memory>
#include <map>
namespace perfect{
namespace common{
//!
//! \brief The InterfaceMessage class is an interface which is used for creating message for communication between software modules,
//!  peripheral devices,... etc. totaly it used in message level.
//!

//This interface will be must convert to preprocessing config


class InterfaceMessage
{
public :
    //!
    //! \brief getType method return message type which the show the funcationality of message and its properties
    //! \return int which can cast to module enum
    //!
    virtual int get_type() const = 0;
    //!
    //! \brief getType method return message id, that used for every message separete from other same messages
    //! \return MessageType which is enum type
    //!
    virtual int get_id() const = 0;

    virtual ~InterfaceMessage() {} //! interface virtual destructor
};


//class AbstractProcessMessage : public std::enable_shared_from_this<AbstractProcessMessage>{
//public:
//    AbstractProcessMessage(std::map<int, std::function<void (std::shared_ptr<common::InterfaceMessage>)>> processes ) : processes_(processes) { }
//    std::shared_ptr<AbstractProcessMessage> get_shared() { return shared_from_this(); }
//    void init_my_self() { }
//private:
//    std::map<int, std::function<void (std::shared_ptr<common::InterfaceMessage>) >> processes_;
//};

} // common
} // perfect
