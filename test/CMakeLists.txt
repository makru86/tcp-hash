function(create_test testname)

    add_executable(test_${testname}
        main.cpp
        test_${testname}.cpp
    )

    target_link_libraries(test_${testname} PRIVATE
            libtcp_hash
            ${Boost_LIBRARIES}
            )

    add_test(NAME test_${testname}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_${testname}
    )

endfunction()

create_test("Util")
create_test("Hash")
create_test("Server")