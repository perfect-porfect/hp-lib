cmake_minimum_required(VERSION 3.0.0)
set(TEST_NAME extractor)
#find_package(GTest REQUIRED)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_TYPE Debug)
find_package(Boost REQUIRED COMPONENTS system thread chrono regex filesystem)

#include_directories(${GTEST_INCLUDE_DIRS})

add_executable(extractor extractor.cc )
target_link_libraries(extractor PUBLIC peripheral #[[${GTEST_LIBRARIES}]] ${CMAKE_THREAD_LIBS_INIT})

add_executable(client client.cc)
target_link_libraries(client PUBLIC peripheral #[[${GTEST_LIBRARIES}]] ${CMAKE_THREAD_LIBS_INIT})
