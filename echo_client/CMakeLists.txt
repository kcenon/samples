CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

SET(PROGRAM_NAME echo_client)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED TRUE)

PROJECT(${PROGRAM_NAME})

ADD_EXECUTABLE(${PROGRAM_NAME} echo_client.cpp)

TARGET_INCLUDE_DIRECTORIES(${PROGRAM_NAME} PUBLIC ../messaging_system/utilities)
TARGET_INCLUDE_DIRECTORIES(${PROGRAM_NAME} PUBLIC ../messaging_system/container)
TARGET_INCLUDE_DIRECTORIES(${PROGRAM_NAME} PUBLIC ../messaging_system/threads)
TARGET_INCLUDE_DIRECTORIES(${PROGRAM_NAME} PUBLIC ../messaging_system/network)

ADD_DEPENDENCIES(${PROGRAM_NAME} network)
TARGET_LINK_LIBRARIES(${PROGRAM_NAME} PUBLIC network)