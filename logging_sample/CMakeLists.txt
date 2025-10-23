CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

SET(PROGRAM_NAME logging_sample)
PROJECT(${PROGRAM_NAME})

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create executable
ADD_EXECUTABLE(${PROGRAM_NAME} logging_sample.cpp)

# Link against system libraries
# This sample demonstrates logger_system integration
# Note: These targets are provided by messaging_system's transitive dependencies
TARGET_LINK_LIBRARIES(${PROGRAM_NAME} PRIVATE
    LoggerSystem
    utilities
    fmt::fmt
    Threads::Threads
)

# Set output directory
set_target_properties(${PROGRAM_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Configured ${PROGRAM_NAME}")
