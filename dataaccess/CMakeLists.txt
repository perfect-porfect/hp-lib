cmake_minimum_required(VERSION 3.10.0)

set(MODULE_NAME dataaccess)

file(GLOB SOURCES src/*.cc)
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(include/perfect-lib/dataaccess)

find_package(Threads)
find_package(Boost REQUIRED COMPONENTS system thread regex filesystem)
add_library(${MODULE_NAME} ${SOURCES})

target_include_directories(${MODULE_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
    )
target_link_libraries(${MODULE_NAME} PUBLIC common communication ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
export(TARGETS ${MODULE_NAME} FILE "${MODULE_NAME}.cmake")

option(BUILD_DATAACCESS_TEST "Run all test dataaccess module" ON)
if (BUILD_DATAACCESS_TEST)
    add_subdirectory(test)
endif()
