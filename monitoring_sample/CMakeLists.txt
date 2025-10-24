CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

SET(PROGRAM_NAME monitoring_sample)
PROJECT(${PROGRAM_NAME})

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create executable
ADD_EXECUTABLE(${PROGRAM_NAME} monitoring_sample.cpp)

# Link libraries (only need threads - this is a standalone sample)
TARGET_LINK_LIBRARIES(${PROGRAM_NAME} PRIVATE
    Threads::Threads
)

# Set output directory
set_target_properties(${PROGRAM_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Configured ${PROGRAM_NAME}")
