CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

SET(PROGRAM_NAME udp_echo_client)
PROJECT(${PROGRAM_NAME})

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create executable
ADD_EXECUTABLE(${PROGRAM_NAME} udp_echo_client.cpp)

# Add include directories for asio (required by NetworkSystem UDP headers)
TARGET_INCLUDE_DIRECTORIES(${PROGRAM_NAME} PRIVATE
    /opt/homebrew/include
)

# Link libraries
TARGET_LINK_LIBRARIES(${PROGRAM_NAME} PRIVATE
    NetworkSystem
    LoggerSystem
    utilities
    interfaces
    thread_base
    Threads::Threads
)

# Set output directory
set_target_properties(${PROGRAM_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Configured ${PROGRAM_NAME}")
