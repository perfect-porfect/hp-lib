#include <iostream>
#include <boost/thread.hpp>

#include "hp/common/buffer/circular_buffer.h"
#include <thread>

using namespace hp::peripheral;

int main()
{
    auto main_buffer = std::make_shared<CircularBuffer>(1 * 1024 * 1024);
    auto copy_buffer = std::make_shared<CircularBuffer>(1 * 1024 * 1024);
    main_buffer->write("abc", 3);
    std::string main_data = main_buffer->get_all_bytes();
    copy_buffer->write(main_data.data(), main_data.size());
    std::string fuck_data = copy_buffer->get_all_bytes();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    main_buffer->erase_buffer();
    copy_buffer->erase_buffer();
    while(1)
        std::this_thread::sleep_for(std::chrono::seconds(10));

//    3
    //    std::vector<char*> all_data;
//    all_data.resize();
//    for (auto data: all_data) {
//        data = nullptr;
//    }
//    const uint32_t buffer_size = 1 * 1024 * 1024 * 1024;
//    for (auto data: all_data) {
//        data = new char[buffer_size];

//        for (uint32_t index = 0 ; index < buffer_size; index++)
//            data[index] = 0xaa;

//        if (data == nullptr) {
//            data[100] = 0x02;
//            std::cout << "fuck data ok" << data[100] <<  std::endl;
//        } else {
//            std::cout << "created " << std::endl;
//        }
//    }
}
