add_library(libtcp_hash STATIC hash.cpp hash.h server.cpp server.h util.h)

target_link_libraries(libtcp_hash PUBLIC
        Boost::system
        Threads::Threads
        coverage_config
        xxHash::xxHash)

target_include_directories(libtcp_hash PUBLIC "${CMAKE_SOURCE_DIR}")

function(create_test testname)
    add_executable(test_${testname} main.test.cpp ${testname}.test.cpp )
    target_link_libraries(test_${testname} PRIVATE
            libtcp_hash
            ${Boost_LIBRARIES})
    add_test(NAME test_${testname}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_${testname})
endfunction()

if (BUILD_UNIT_TESTS)
    find_package(Boost REQUIRED COMPONENTS unit_test_framework)
    enable_testing()
    create_test("util")
    create_test("hash")
    create_test("server")
endif ()

if (BUILD_LOAD_TESTS)
    find_package(benchmark REQUIRED)
    add_executable(loadtest main.loadtest.cpp)
    target_link_libraries(loadtest PRIVATE
            benchmark::benchmark
            benchmark::benchmark_main
            ${Boost_LIBRARIES}
            libtcp_hash)
endif ()
