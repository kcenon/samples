CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

SET(PROGRAM_NAME database_integration_sample)
PROJECT(${PROGRAM_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
ADD_EXECUTABLE(${PROGRAM_NAME}
    database_integration_sample.cpp
)

# Add include directories
# database_system's header files are in ../database_system/database/
target_include_directories(${PROGRAM_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../database_system/database
    ${CMAKE_CURRENT_SOURCE_DIR}/../../database_system
)

# Link with required libraries
# database provides database functionality (not database_system)
# LoggerSystem provides logging (via messaging_system dependency chain)
# thread_base, interfaces, utilities provide thread_system components
TARGET_LINK_LIBRARIES(${PROGRAM_NAME} PRIVATE
    database             # Database operations (from database_system/database)
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
