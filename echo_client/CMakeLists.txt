CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

SET(PROGRAM_NAME echo_client)
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED TRUE)

PROJECT(${PROGRAM_NAME})

ADD_EXECUTABLE(${PROGRAM_NAME} echo_client.cpp)

TARGET_INCLUDE_DIRECTORIES(${PROGRAM_NAME} PUBLIC 
    ../messaging_system/thread_system/sources/utilities
    ../messaging_system/thread_system/sources/utilities/parsing
    ../messaging_system
    ../messaging_system/container
    ../messaging_system/network
)

ADD_DEPENDENCIES(${PROGRAM_NAME} utilities container network)
TARGET_LINK_LIBRARIES(${PROGRAM_NAME} PUBLIC utilities container network fmt::fmt)