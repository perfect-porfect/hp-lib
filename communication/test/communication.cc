#include <iostream>
#include <vector>
#include <gtest/gtest.h>
#include <thread>
#include <perfect-lib/communication/modules_communication_manager.h>
#include <functional>


void hassan(perfect::Shared_MSG msg, perfect::ModuleName src) {
    int counter = 0;
    while (1) {
        if (counter >= 5)
            break;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        counter++;
    }
}

void ehsan(perfect::Shared_MSG msg, perfect::ModuleName src){
    int counter = 0;
    while (1) {
        if (counter >= 5)
            break;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        counter++;
    }
}


TEST(Communication, Sendsync)
{
    auto com_manager = std::make_shared<perfect::communication::ModulesCommunicationManager>();
    com_manager->add_connection(std::bind(ehsan, std::placeholders::_1, std::placeholders::_2), perfect::ModuleName::Communication);
    com_manager->add_connection(std::bind(hassan, std::placeholders::_1, std::placeholders::_2), perfect::ModuleName::DataAccess);
    com_manager->send_message(nullptr, perfect::ModuleName::Communication, perfect::ModuleName::DataAccess);
    com_manager->send_message(nullptr, perfect::ModuleName::DataAccess, perfect::ModuleName::Communication);
}

TEST(Communication, SendAsync)
{
    auto com_manager = std::make_shared<perfect::communication::ModulesCommunicationManager>();
    com_manager->add_connection(std::bind(ehsan, std::placeholders::_1, std::placeholders::_2), perfect::ModuleName::Communication);
    com_manager->add_connection(std::bind(hassan, std::placeholders::_1, std::placeholders::_2), perfect::ModuleName::DataAccess);
    com_manager->send_message_async(nullptr, perfect::ModuleName::Communication, perfect::ModuleName::DataAccess);
    com_manager->send_message_async(nullptr, perfect::ModuleName::DataAccess, perfect::ModuleName::Communication);
  }


int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    int result =  RUN_ALL_TESTS();
    std::cout<<"Finished all test cases"<<std::endl;
    return result;
}
