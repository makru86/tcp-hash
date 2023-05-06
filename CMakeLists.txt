cmake_minimum_required(VERSION 3.16)

project(tcp_hash)

option(BUILD_TESTS "Build unit tests (default OFF)" OFF)
option(CODE_COVERAGE "Enable coverage for GCC & LLVM/Clang compilers (default OFF)" OFF)
option(ASIO_ENABLE_HANDLER_TRACKING "Enable ASIO handler tracking (default OFF)" OFF)
option(ENABLE_DEBUG_LOG "Enable debug log (default OFF)" OFF)
option(ENABLE_GPROF_PROFILING "Enable Gprof profiling (default OFF)" OFF)

message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Code coverage: ${CODE_COVERAGE}")
message(STATUS "ASIO handler tracking: ${ASIO_ENABLE_HANDLER_TRACKING}")
message(STATUS "Enable debug log: ${ENABLE_DEBUG_LOG}")
message(STATUS "Enable Gprof profiling: ${ENABLE_GPROF_PROFILING}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system unit_test_framework)
find_package(xxHash REQUIRED)

# TODO refactor C++ sources to use coroutines instead of callbacks
add_library(coroutine_config INTERFACE)
if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_compile_options(coroutine_config INTERFACE /await)
elseif (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    target_compile_options(coroutine_config INTERFACE -std=c++17 -fcoroutines-ts)
    target_compile_options(coroutine_config INTERFACE -std=c++20)
elseif (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    target_compile_options(coroutine_config INTERFACE -std=c++20 -fcoroutines)
endif ()

add_library(coverage_config INTERFACE)
if (CODE_COVERAGE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(coverage_config INTERFACE
                -O0        # no optimization
                -g         # generate debug info
                --coverage # sets all required flags
                )
        target_link_options(coverage_config INTERFACE --coverage)
    else ()
        message(WARNING "coverage_config")
    endif ()
endif ()

add_library(asio_tracking_config INTERFACE)
if (ASIO_ENABLE_HANDLER_TRACKING)
    target_compile_definitions(asio_tracking_config INTERFACE BOOST_ASIO_ENABLE_HANDLER_TRACKING)
endif ()

add_library(debug_log_config INTERFACE)
if (ENABLE_DEBUG_LOG)
    target_compile_definitions(debug_log_config INTERFACE ENABLE_DEBUG_LOG)
endif ()

add_library(gprof_config INTERFACE)
if (ENABLE_GPROF_PROFILING)
    if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
        target_compile_options(gprof_config INTERFACE -pg)
        target_link_options(gprof_config INTERFACE -pg)
    else ()
        message(WARNING "gprof_config")
    endif ()
endif ()

add_subdirectory(libtcp_hash)
add_subdirectory(tcp_hash)
