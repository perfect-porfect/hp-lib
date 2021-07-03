#pragma once

namespace perfect {
namespace dataaccess {
namespace messages {

enum class MessageType{
    //DataAccess
    FileManager,
    READ_FILE = 0x8585,
    WRITE_FILE = 0x9595,

    DataBase  = 0xffff,

    LogManagment = 0xfffffff,

};

} // messages
} // dataaccess
} // perfect
