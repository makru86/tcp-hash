function(create_test testname)

    add_executable(test_${testname}
        main.cpp
        test_${testname}.cpp
    )

    target_link_libraries(test_${testname} PRIVATE
            MyLib
            ${Boost_LIBRARIES}
            gtest
            gmock
            )

    add_test(NAME test_${testname}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_${testname}
    )

endfunction()

create_test("TcpServer")
create_test("Log")
create_test("SignalHandler")
create_test("Hash")