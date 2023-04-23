cmake_minimum_required(VERSION 3.16)

project(tcp_hash)

option(BUILD_UNIT_TESTS "Build unit tests (default OFF)" OFF)
option(BUILD_LOAD_TESTS "Build load tests (default OFF)" OFF)
option(CODE_COVERAGE "Enable coverage reporting (default OFF)" OFF)

message(STATUS "Build unit tests: ${BUILD_UNIT_TESTS}")
message(STATUS "Build load tests: ${BUILD_LOAD_TESTS}")
message(STATUS "Code coverage: ${CODE_COVERAGE}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(coverage_config INTERFACE)

if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Add required flags (GCC & LLVM/Clang)
    target_compile_options(coverage_config INTERFACE
        -O0        # no optimization
        -g         # generate debug info
        --coverage # sets all required flags
    )

    target_link_options(coverage_config INTERFACE --coverage)
endif(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(xxHash REQUIRED)

add_subdirectory(src)

if (BUILD_UNIT_TESTS)
    find_package(Boost REQUIRED COMPONENTS unit_test_framework)
    enable_testing()
    add_subdirectory(test)
endif()

if (BUILD_LOAD_TESTS)
    add_subdirectory(loadtest)
endif()