CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

SET(PROGRAM_NAME thread_integration_sample)
PROJECT(${PROGRAM_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
ADD_EXECUTABLE(${PROGRAM_NAME}
    thread_integration_sample.cpp
)

# Link with required libraries
# LoggerSystem provides logging (via messaging_system dependency chain)
# thread_base, interfaces, utilities provide thread_system components
TARGET_LINK_LIBRARIES(${PROGRAM_NAME} PRIVATE
    LoggerSystem         # Logging functionality
    thread_base          # Thread pool and job system
    interfaces           # Thread system interfaces
    utilities            # Common utilities
    fmt::fmt             # String formatting
    Threads::Threads     # Standard threads
)

# Set output directory
set_target_properties(${PROGRAM_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Configured ${PROGRAM_NAME}")
