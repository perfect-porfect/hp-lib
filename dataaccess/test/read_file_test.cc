#include <iostream>
#include <vector>
#include <gtest/gtest.h>
#include <thread>
#include <perfect-lib/communication/modules_communication_manager.h>
#include <functional>
#include <perfect-lib/dataaccess/dataaccessmodule.h>
#include <perfect-lib/dataaccess/messages/read_file.h>
#include <chrono>

TEST(Communication, Sendsync)
{
    auto com_manager = std::make_shared<perfect::communication::ModulesCommunicationManager>();
    auto data_access_module = std::make_shared<perfect::dataaccess::DataAccessModule>(com_manager);
    auto read_file_msg = std::make_shared<perfect::dataaccess::messages::ReadFileMessage>("/home/hp/test.sh", 2);
    com_manager->send_message(read_file_msg, perfect::ModuleName::DataProcess, perfect::ModuleName::DataAccess);
    std::cout << "content: " << read_file_msg->get_content() << std::endl;
}


int main(int argc, char ** argv)
{
    //    testing::InitGoogleTest(&argc, argv);
    //    int result =  RUN_ALL_TESTS();

    auto com_manager = std::make_shared<perfect::communication::ModulesCommunicationManager>();
    auto data_access_module = std::make_shared<perfect::dataaccess::DataAccessModule>(com_manager);
    data_access_module->initialize();
    data_access_module->start();
    auto read_file_msg = std::make_shared<perfect::dataaccess::messages::ReadFileMessage>("/home/hp/test.sh213", 2);
    for (int i = 0 ; i < 10 ; i++) {
        auto t1 = std::chrono::high_resolution_clock::now();
        com_manager->send_message(read_file_msg, perfect::ModuleName::DataProcess, perfect::ModuleName::DataAccess);
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> time_us = t2 - t1;
        std::cout<<"content is: "<<read_file_msg->get_content() <<  " time spend: " << time_us.count() <<  std::endl;
    }
    return 1;
}
