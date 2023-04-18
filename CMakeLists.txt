cmake_minimum_required(VERSION 3.20)

project(CCCBTemplate)

option(BUILD_UNIT_TESTS "Build unit tests (default OFF)" OFF)
option(CODE_COVERAGE "Enable coverage reporting (default OFF)" OFF)

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


add_subdirectory(src)

if (BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()