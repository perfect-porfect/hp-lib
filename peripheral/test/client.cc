#include <iostream>
#include "tcp_client.h"
#include "circular_buffer.h"

using namespace hp::peripheral;

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
        if (!client.is_connected())
            break;

        if (counter == 2)
            client.disconnect();

        counter++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void disconnect_test1() {
    TCPClient client("127.0.0.1", 8585);
    bool con = false;
    while (!con) {
        con = client.connect();
        if (!con)
            std::cout << "Can't connect to tcp server" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    while (1) {
        if (!client.is_connected())
            break;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void disconnect_test2() {
    TCPClient client("127.0.0.1", 8585);
    bool con = false;
    auto buffer = new hp::peripheral::CircularBuffer();
    client.set_buffer(buffer);
    while (!con) {
        con = client.connect();
        if (!con)
            std::cout << "Can't connect to tcp server" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
        std::string data{(char)0xaa, (char)0xff, (char)0xd1, (char)0xd2, (char)0x00, (char)0x00,
                    (char)0x00, (char)0x09, (char)0x01, (char)0x02, (char)0x03, (char)0x04,
                    (char)0x05, (char)0x06, (char)0x07, (char)0x08, (char)0x09, (char)0xaa,
                    (char)0xbb, (char)0xcc, (char)0xdd};
    while (1) {
        if (!client.is_connected())
            break;
        client.send(data.data(), data.length());
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "finished" << std::endl;
    delete buffer;
}

int main()
{
    disconnect_test2();
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
