function(create_test testname)

    add_executable(test_${testname}
        main.cpp
        test_${testname}.cpp
    )

    target_link_libraries(test_${testname} PRIVATE
            MyLib
            ${Boost_LIBRARIES}
    )

    add_test(NAME test_${testname}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_${testname}
    )

endfunction(create_test)


# find Boost package
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

create_test("TcpServer")
create_test("Log")
create_test("SignalHandler")
