#include <iostream>
#include <boost/thread.hpp>

#include "hp/common/buffer/fast_buffer.h"
#include "hp/common/buffer/circular_buffer.h"

void read_()
{
//    auto serial_port =
}

void write_()
{

}



int main()
{
    boost::thread_group thread_group_;
    thread_group_.create_thread(read_);
    thread_group_.create_thread(write_);
    thread_group_.join_all();
}
