CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

SET(PROGRAM_NAME combined_sample)

# Collect source files
FILE(GLOB SOURCES "*.cpp")

# Create executable
ADD_EXECUTABLE(${PROGRAM_NAME} ${SOURCES})

# Link libraries
TARGET_LINK_LIBRARIES(${PROGRAM_NAME} PRIVATE
    LoggerSystem
    container_system
    utilities
    interfaces
    thread_base
    fmt::fmt
    Threads::Threads
)

# Install
INSTALL(TARGETS ${PROGRAM_NAME}
        RUNTIME DESTINATION bin)
