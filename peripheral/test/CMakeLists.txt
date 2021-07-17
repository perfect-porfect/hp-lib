cmake_minimum_required(VERSION 3.0.0)
set(TEST_NAME extractor)
#find_package(GTest REQUIRED)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_TYPE Debug)
find_package(Boost REQUIRED COMPONENTS system thread chrono regex filesystem)

#include_directories(${GTEST_INCLUDE_DIRS})

add_executable(full_packet full_packet.cc )
target_link_libraries(full_packet PUBLIC common peripheral #[[${GTEST_LIBRARIES}]] ${CMAKE_THREAD_LIBS_INIT})


add_executable(packet1 packet1.cc)
target_link_libraries(packet1 PUBLIC common peripheral #[[${GTEST_LIBRARIES}]] ${CMAKE_THREAD_LIBS_INIT})

add_executable(packet2 packet2.cc)
target_link_libraries(packet2 PUBLIC common peripheral #[[${GTEST_LIBRARIES}]] ${CMAKE_THREAD_LIBS_INIT})

add_executable(client client.cc)
target_link_libraries(client PUBLIC common peripheral #[[${GTEST_LIBRARIES}]] ${CMAKE_THREAD_LIBS_INIT})

add_executable(buffer buffer.cc)
target_link_libraries(buffer PUBLIC common peripheral #[[${GTEST_LIBRARIES}]] ${CMAKE_THREAD_LIBS_INIT})

add_executable(buffer1 buffer1.cc)
target_link_libraries(buffer1 PUBLIC common peripheral #[[${GTEST_LIBRARIES}]] ${CMAKE_THREAD_LIBS_INIT})


add_executable(send_receive send_receive.cc)
target_link_libraries(send_receive PUBLIC common peripheral #[[${GTEST_LIBRARIES}]] ${CMAKE_THREAD_LIBS_INIT})
