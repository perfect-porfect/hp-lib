#include <iostream>
#include <boost/thread.hpp>

#include "hp/common/buffer/circular_buffer.h"

const int Counter = 100000000;
void read_(hp::peripheral::AbstractBuffer* buffer) {
    int counter = 0;
    while(1) {
        uint8_t data = buffer->read_next_byte();
        if (counter % 100000 == 0)
            std::cout << "read #" << counter << std::endl;
        if (counter == Counter) {
            std::cout << "read #" << counter << " finished" << std::endl;
            break;
        }
        counter++;
    }
}

void write_(hp::peripheral::AbstractBuffer* buffer)
{
    int counter = 0;
    uint8_t *data = new u_int8_t[1];
    data[0] = 0x61;
    while(1) {
        uint8_t data1 = buffer->write(data, 1);
        if (counter % 1000000 == 0)
            std::cout << "write #" << counter << std::endl;
        if (counter == Counter) {
            std::cout << "write #" << counter << " finished" << std::endl;
            break;
        }
        counter++;
    }
}



int main()
{
    boost::thread_group thread_group_;
    auto buffer = new hp::peripheral::CircularBuffer(4 * 1024 * 1024);

    thread_group_.create_thread(std::bind(read_, buffer));
    thread_group_.create_thread(std::bind(write_, buffer));
    thread_group_.join_all();


}
