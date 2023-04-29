cmake_minimum_required(VERSION 3.16)

project(tcp_hash)

option(BUILD_UNIT_TESTS "Build unit tests (default OFF)" OFF)
option(BUILD_LOAD_TESTS "Build load tests (default OFF)" OFF)
option(CODE_COVERAGE "Enable coverage for GCC & LLVM/Clang compilers (default OFF)" OFF)
option(ASIO_ENABLE_HANDLER_TRACKING "Enable ASIO handler tracking (default OFF)" OFF)

message(STATUS "Build unit tests: ${BUILD_UNIT_TESTS}")
message(STATUS "Build load tests: ${BUILD_LOAD_TESTS}")
message(STATUS "Code coverage: ${CODE_COVERAGE}")
message(STATUS "ASIO handler tracking: ${ASIO_ENABLE_HANDLER_TRACKING}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(coverage_config INTERFACE)

if (CODE_COVERAGE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # Add required flags
        target_compile_options(coverage_config INTERFACE
                -O0        # no optimization
                -g         # generate debug info
                --coverage # sets all required flags
                )
        target_link_options(coverage_config INTERFACE --coverage)
    else ()
        message(FATAL_ERROR "For your compiler '${CMAKE_CXX_COMPILER_ID}' code coverage is not implemented. Sorry.")
    endif ()
endif ()

add_library(asio_config INTERFACE)
if (ASIO_ENABLE_HANDLER_TRACKING)
    target_compile_definitions(asio_config INTERFACE BOOST_ASIO_ENABLE_HANDLER_TRACKING)
endif ()


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(xxHash REQUIRED)

add_subdirectory(libtcp_hash)
add_subdirectory(tcp_hash)