cmake_minimum_required(VERSION 3.0.0)

project(hp-lib VERSION 0.1 LANGUAGES CXX)

add_subdirectory(common)
add_subdirectory(peripheral)
#add_subdirectory(communication)
#add_subdirectory(dataaccess)
#add_subdirectory(controller)
#add_subdirectory(signalprocessing)
#add_subdirectory(dataprocessing)
#add_subdirectory(simulator)

#add_subdirectory(src)
#add_subdirectory(test)
#add_subdirectory(gui)
#add_subdirectory(libs)


#option(PROJECT_BUILD_MODE "Set Debug Mode" ON)
#set(CMAKE_BUILD_TYPE "Release")
#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} "src/main.cpp")
#target_link_libraries(${PROJECT_NAME} PUBLIC communication ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
