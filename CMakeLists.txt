CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

# Project declaration
SET(PROJECT_NAME samples)
PROJECT(${PROJECT_NAME} VERSION 1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output information
message(STATUS "========================================")
message(STATUS "Configuring ${PROJECT_NAME}")
message(STATUS "========================================")
message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")

# Platform-specific settings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00) # Windows 10
elseif(APPLE)
    add_definitions(-DAPPLE_PLATFORM)
endif()

# ========================================
# Dependency Management Strategy
# ========================================
# This project supports three modes for obtaining dependencies:
# 1. FetchContent (default) - Automatically downloads from GitHub
# 2. Local systems - Uses local repositories
# 3. Installed systems - Uses find_package()

option(SAMPLES_USE_LOCAL_SYSTEMS "Use local system repositories instead of FetchContent" OFF)
option(SAMPLES_USE_INSTALLED_SYSTEMS "Use installed system packages" OFF)
option(SAMPLES_BUILD_EXAMPLES "Build sample programs" ON)

# Set CMAKE_TOOLCHAIN_FILE for vcpkg if not already set
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
    message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(Threads REQUIRED)
find_package(fmt CONFIG REQUIRED)

# ========================================
# System Dependencies
# ========================================

include(FetchContent)

# Helper function to add system dependency
function(add_system_dependency SYSTEM_NAME GITHUB_REPO)
    if(SAMPLES_USE_INSTALLED_SYSTEMS)
        # Mode 3: Use installed packages
        message(STATUS "Looking for installed ${SYSTEM_NAME}...")
        find_package(${SYSTEM_NAME} REQUIRED)

    elseif(SAMPLES_USE_LOCAL_SYSTEMS)
        # Mode 2: Use local repositories
        set(LOCAL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../${SYSTEM_NAME}")
        if(EXISTS "${LOCAL_PATH}")
            message(STATUS "Using local ${SYSTEM_NAME} from: ${LOCAL_PATH}")

            # Configure system to build as dependency (disable tests/examples/samples)
            set(BUILD_${SYSTEM_NAME}_TESTS OFF CACHE BOOL "Disable tests for ${SYSTEM_NAME}" FORCE)
            set(BUILD_${SYSTEM_NAME}_EXAMPLES OFF CACHE BOOL "Disable examples for ${SYSTEM_NAME}" FORCE)
            set(BUILD_${SYSTEM_NAME}_SAMPLES OFF CACHE BOOL "Disable samples for ${SYSTEM_NAME}" FORCE)

            # For messaging_system, propagate local mode to its dependencies
            if(SYSTEM_NAME STREQUAL "messaging_system")
                set(MESSAGING_USE_LOCAL_SYSTEMS ON CACHE BOOL "Use local systems for messaging_system" FORCE)
            endif()

            add_subdirectory(${LOCAL_PATH} ${CMAKE_BINARY_DIR}/_deps/${SYSTEM_NAME}-build)
        else()
            message(FATAL_ERROR "Local ${SYSTEM_NAME} not found at: ${LOCAL_PATH}")
        endif()

    else()
        # Mode 1: Use FetchContent (default)
        message(STATUS "Fetching ${SYSTEM_NAME} from GitHub...")
        FetchContent_Declare(
            ${SYSTEM_NAME}
            GIT_REPOSITORY https://github.com/${GITHUB_REPO}.git
            GIT_TAG main
            GIT_SHALLOW TRUE
        )

        # Configure system to build as dependency
        set(BUILD_${SYSTEM_NAME}_TESTS OFF CACHE BOOL "Disable tests for ${SYSTEM_NAME}" FORCE)
        set(BUILD_${SYSTEM_NAME}_EXAMPLES OFF CACHE BOOL "Disable examples for ${SYSTEM_NAME}" FORCE)
        set(BUILD_${SYSTEM_NAME}_SAMPLES OFF CACHE BOOL "Disable samples for ${SYSTEM_NAME}" FORCE)

        FetchContent_MakeAvailable(${SYSTEM_NAME})
    endif()
endfunction()

# ========================================
# Global Configuration for Dependencies
# ========================================
# Disable tests, examples, and samples for all dependencies
set(BUILD_TESTING OFF CACHE BOOL "Disable testing for dependencies" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Disable examples for dependencies" FORCE)
set(BUILD_SAMPLES OFF CACHE BOOL "Disable samples for dependencies" FORCE)

# System-specific configurations
set(COMMON_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(COMMON_BUILD_INTEGRATION OFF CACHE BOOL "" FORCE)
set(COMMON_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(THREAD_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(THREAD_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(THREAD_BUILD_EXAMPLES OFF FORCE)

# Logger system configuration
set(LOGGER_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LOGGER_BUILD_INTEGRATION_TESTS OFF CACHE BOOL "" FORCE)
set(LOGGER_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LOGGER_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)

set(CONTAINER_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CONTAINER_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(CONTAINER_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONTAINER_BUILD_DOCS OFF CACHE BOOL "" FORCE)

set(NETWORK_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(NETWORK_BUILD_INTEGRATION_TESTS OFF CACHE BOOL "" FORCE)
set(NETWORK_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(NETWORK_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(MESSAGING_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(MESSAGING_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(MESSAGING_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add required systems
message(STATUS "========================================")
message(STATUS "Resolving System Dependencies")
message(STATUS "========================================")

if(SAMPLES_USE_INSTALLED_SYSTEMS)
    message(STATUS "Mode: Using installed system packages")
elseif(SAMPLES_USE_LOCAL_SYSTEMS)
    message(STATUS "Mode: Using local system repositories")
    message(STATUS "Local base path: ${CMAKE_CURRENT_SOURCE_DIR}/..")
else()
    message(STATUS "Mode: Using FetchContent (downloading from GitHub)")
endif()

# Messaging system (complete integration)
# This brings in all other systems transitively:
# - common_system, thread_system, logger_system
# - container_system, network_system
# No need to add them separately to avoid target duplication
add_system_dependency(messaging_system kcenon/messaging_system)

# Fix ABI compatibility: Manually add USE_THREAD_SYSTEM_INTEGRATION to LoggerSystem
# messaging_system builds logger_system in standalone mode to avoid duplicating thread_system,
# but this causes ABI mismatch with network_system which expects integration mode.
# Solution: Keep standalone mode (to avoid duplicate thread_system) but define the integration macro
# and manually add thread_system include directories and link libraries.
if(TARGET LoggerSystem)
    target_compile_definitions(LoggerSystem PUBLIC USE_THREAD_SYSTEM_INTEGRATION)
    # Add thread_system include directory
    target_include_directories(LoggerSystem PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../thread_system/include>
    )
    # Link with thread_system libraries
    if(TARGET utilities AND TARGET interfaces AND TARGET thread_base)
        target_link_libraries(LoggerSystem PUBLIC utilities interfaces thread_base)
    endif()
    message(STATUS "Added USE_THREAD_SYSTEM_INTEGRATION to LoggerSystem for ABI compatibility")
endif()

message(STATUS "========================================")
message(STATUS "All dependencies resolved successfully")
message(STATUS "========================================")

# ========================================
# Sample Programs
# ========================================

if(SAMPLES_BUILD_EXAMPLES)
    message(STATUS "Building sample programs...")

    # Add sample subdirectories
    ADD_SUBDIRECTORY(logging_sample)
    ADD_SUBDIRECTORY(container_sample)
    ADD_SUBDIRECTORY(threads_sample)
    ADD_SUBDIRECTORY(echo_client)
    ADD_SUBDIRECTORY(echo_server)

    message(STATUS "Sample programs configured")
endif()

# ========================================
# Installation
# ========================================

install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
        DESTINATION bin
        FILES_MATCHING PATTERN "*sample*" PATTERN "*client*" PATTERN "*server*")

# ========================================
# Summary
# ========================================

message(STATUS "========================================")
message(STATUS "${PROJECT_NAME} Configuration Summary")
message(STATUS "========================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Dependency mode: ${SAMPLES_USE_LOCAL_SYSTEMS} (local) / ${SAMPLES_USE_INSTALLED_SYSTEMS} (installed)")
message(STATUS "Build examples: ${SAMPLES_BUILD_EXAMPLES}")
message(STATUS "========================================")
