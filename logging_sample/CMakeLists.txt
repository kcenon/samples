CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

SET(PROGRAM_NAME logging_sample)
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED TRUE)

PROJECT(${PROGRAM_NAME})

ADD_EXECUTABLE(${PROGRAM_NAME} logging_sample.cpp)

TARGET_INCLUDE_DIRECTORIES(${PROGRAM_NAME} PUBLIC ../messaging_system/thread_system/sources/utilities)
TARGET_INCLUDE_DIRECTORIES(${PROGRAM_NAME} PUBLIC ../messaging_system/thread_system/sources/utilities/parsing)
TARGET_INCLUDE_DIRECTORIES(${PROGRAM_NAME} PUBLIC ../messaging_system/thread_system/sources/logger)

ADD_DEPENDENCIES(${PROGRAM_NAME} utilities logger)
TARGET_LINK_LIBRARIES(${PROGRAM_NAME} PUBLIC utilities logger fmt::fmt)