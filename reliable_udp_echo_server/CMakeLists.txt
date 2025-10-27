CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

SET(PROGRAM_NAME reliable_udp_echo_server)
PROJECT(${PROGRAM_NAME})

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create executable
ADD_EXECUTABLE(${PROGRAM_NAME} reliable_udp_echo_server.cpp)

# Link against system libraries
# This sample demonstrates network_system reliable UDP server
TARGET_LINK_LIBRARIES(${PROGRAM_NAME} PRIVATE
    NetworkSystem
    LoggerSystem
    utilities
    interfaces
    thread_base
    fmt::fmt
    Threads::Threads
)

# NetworkSystem requires ASIO headers for its public API
# Find ASIO include directory from network_system's FetchContent
if(EXISTS "${CMAKE_BINARY_DIR}/_deps/network_system_asio-src/asio/include")
    target_include_directories(${PROGRAM_NAME} SYSTEM PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/network_system_asio-src/asio/include"
    )
elseif(EXISTS "${CMAKE_BINARY_DIR}/_deps/network_system-build/_deps/network_system_asio-src/asio/include")
    target_include_directories(${PROGRAM_NAME} SYSTEM PRIVATE
        "${CMAKE_BINARY_DIR}/_deps/network_system-build/_deps/network_system_asio-src/asio/include"
    )
elseif(EXISTS "/opt/homebrew/include")
    # Fallback to system ASIO (macOS Homebrew)
    target_include_directories(${PROGRAM_NAME} SYSTEM PRIVATE
        "/opt/homebrew/include"
    )
else()
    message(WARNING "Could not find ASIO headers for ${PROGRAM_NAME}")
endif()

# Set output directory
set_target_properties(${PROGRAM_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Configured ${PROGRAM_NAME}")
