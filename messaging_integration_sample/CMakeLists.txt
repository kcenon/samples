CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

SET(PROGRAM_NAME messaging_integration_sample)
PROJECT(${PROGRAM_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
ADD_EXECUTABLE(${PROGRAM_NAME}
    messaging_integration_sample.cpp
)

# Add include directories for messaging_system
target_include_directories(${PROGRAM_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../messaging_system/application_layer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../common_system/include
)

# Link with required libraries
# messaging_client provides the message bus and pub/sub functionality
# LoggerSystem provides logging (via messaging_system dependency chain)
TARGET_LINK_LIBRARIES(${PROGRAM_NAME} PRIVATE
    messaging_services       # High-level message bus API (application_layer)
    LoggerSystem             # Logging functionality
    fmt::fmt                 # String formatting
    Threads::Threads         # Standard threads
)

# Set output directory
set_target_properties(${PROGRAM_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Configured ${PROGRAM_NAME}")
