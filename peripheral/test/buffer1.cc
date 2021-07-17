#include <iostream>
#include <boost/thread.hpp>

#include "hp/common/buffer/fast_buffer.h"
#include "hp/common/buffer/circular_buffer.h"
#include <thread>

using namespace hp::peripheral;

CircularBuffer Buffer(10);
void write_buffer() {
    int size = 15;
    std::string data;
    data.resize(size);
    for (int i = 0; i < size; i++)
        data[i] = i;

    while(1) {
        Buffer.write((uint8_t*)data.data(), size);
        std::cout <<"write data" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void read_buffer() {
    while (1) {
        int size = 2;
        std::string data;
        data.resize(size);
        Buffer.read((uint8_t*)data.data(), size);
        std::cout <<"read data" << std::endl;
    }
}
boost::thread_group all_threads;

int main()
{
    all_threads.create_thread(write_buffer);
    all_threads.create_thread(read_buffer);
    all_threads.join_all();

}
