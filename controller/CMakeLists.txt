cmake_minimum_required(VERSION 3.0.0)

set(MODULE_NAME controller)

#add_subdirectory(test)
#add_subdirectory(libs)

file(GLOB Srcs src/*.cc)
set(SOURCES "")
set(SOURCES ${SOURCES} ${Srcs})
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Threads)
find_package(Boost REQUIRED COMPONENTS system thread regex filesystem)
include_directories(include/perfect-lib/controller)

add_library(${MODULE_NAME} STATIC ${SOURCES})
target_link_libraries(${MODULE_NAME} PUBLIC common communication dataaccess ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(${MODULE_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
)

export(TARGETS ${MODULE_NAME} FILE "${PROJECT_NAME}.cmake")
