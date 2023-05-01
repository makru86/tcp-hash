cmake_minimum_required(VERSION 3.16)

project(tcp_hash)

option(BUILD_UNIT_TESTS "Build unit tests (default OFF)" OFF)
option(BUILD_LOAD_TESTS "Build load tests (default OFF)" OFF)
option(CODE_COVERAGE "Enable coverage for GCC & LLVM/Clang compilers (default OFF)" OFF)
option(ASIO_ENABLE_HANDLER_TRACKING "Enable ASIO handler tracking (default OFF)" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(xxHash REQUIRED)

message(STATUS "Build unit tests: ${BUILD_UNIT_TESTS}")
message(STATUS "Build load tests: ${BUILD_LOAD_TESTS}")
message(STATUS "Code coverage: ${CODE_COVERAGE}")
message(STATUS "ASIO handler tracking: ${ASIO_ENABLE_HANDLER_TRACKING}")

add_library(coroutine_config INTERFACE)
if (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_compile_options(coroutine_config INTERFACE /await)
elseif (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    target_compile_options(coroutine_config INTERFACE -std=c++17 -fcoroutines-ts)
    target_compile_options(coroutine_config INTERFACE -std=c++20)
elseif (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    message(WARNING "coroutine_config")
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

add_library(asio_config INTERFACE)
if (ASIO_ENABLE_HANDLER_TRACKING)
    target_compile_definitions(asio_config INTERFACE BOOST_ASIO_ENABLE_HANDLER_TRACKING)
endif ()

add_executable(cc cc.cc)
target_link_libraries(cc PRIVATE coroutine_config)

#add_subdirectory(libtcp_hash)
#add_subdirectory(tcp_hash)