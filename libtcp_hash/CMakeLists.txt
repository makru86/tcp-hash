# glob all .cpp and .hpp files excluding test.cpp
set(LIBRARY_SOURCES
        client.cpp
        client.hpp
        handler_allocator.hpp
        hash.cpp
        hash.hpp
        server.hpp
        util.cpp
        util.hpp
        )

add_library(libtcp_hash STATIC ${LIBRARY_SOURCES})
target_include_directories(libtcp_hash PUBLIC "${CMAKE_SOURCE_DIR}")
target_link_libraries(libtcp_hash PUBLIC
        Boost::system
        Threads::Threads
        xxHash::xxHash
        # Library configuration options:
        coverage_config
        coroutine_config
        asio_tracking_config
        debug_log_config
        gprof_config
        )

function(create_test testname)
    add_executable(test_${testname}
            main.test.cpp
            ${testname}.test.cpp
            )
    target_link_libraries(test_${testname} PRIVATE
            libtcp_hash
            ${Boost_LIBRARIES}
            )
    add_test(NAME test_${testname}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_${testname})
endfunction()

function(create_loadtest testname)
    add_executable(loadtest_${testname}
            ${testname}.loadtest.cpp
            #            main.test.cpp
            #            main.loadtest.cpp
            )
    target_link_libraries(loadtest_${testname} PRIVATE
            benchmark::benchmark
            benchmark::benchmark_main
            libtcp_hash
            ${Boost_LIBRARIES}
            )
    add_test(NAME loadtest_${testname}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/loadtest_${testname})

endfunction()

if (BUILD_TESTS)
    find_package(Boost REQUIRED COMPONENTS unit_test_framework)
    find_package(benchmark REQUIRED)
    enable_testing()

    # Unit tests
    create_test("util")
    create_test("hash")
    create_test("server")

    # Load test
    create_loadtest("server")
endif ()
