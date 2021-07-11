#include <iostream>
#include "tcpclient.h"

using namespace hp::peripheral;

// aa ff d1 d2 00 00 00 09 01 02 03 04 05 06 07 08 09 aa bb cc dd

void disconnect_test() {
    TCPClient client("127.0.0.1", 8585);
    bool con = false;
    while(!con) {
        con = client.connect();
        if (!con)
            std::cout << "Can't connect to tcp server" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    int counter = 0;
    while (1) {
        if (!client.is_connected()) {
            std::cout << "1" << std::endl;
            break;
        }
        if (counter == 2) {
            std::cout << "2" << std::endl;
            client.disconnect();
        }
        std::cout << "3" << std::endl;
        counter++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    disconnect_test();
    //    TCPClient client("127.0.0.1", 8585);
    //    bool con = false;
    //    while(!con) {
    //        bool con = client.connect();
    //        if (!con)
    //            std::cout << "Cant connect to tcp server" << std::endl;
    //        std::this_thread::sleep_for(std::chrono::seconds(1));
    //    }
    //    std::string data{(char)0xaa, (char)0xff, (char)0xd1, (char)0xd2, (char)0x00, (char)0x00,
    //                (char)0x00, (char)0x09, (char)0x01, (char)0x02, (char)0x03, (char)0x04,
    //                (char)0x05, (char)0x06, (char)0x07, (char)0x08, (char)0x09, (char)0xaa,
    //                (char)0xbb, (char)0xcc, (char)0xdd};
    //    int counter = 0;
    //    while (1) {
    //        if (!client.is_connected())
    //            break;
    //        //        client.send(data.data(), data.size());
    //        std::this_thread::sleep_for(std::chrono::seconds(1));
    //    }

}
