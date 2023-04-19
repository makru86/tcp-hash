function(create_test testname)

    add_executable(test_${testname}
        test_${testname}.cpp
    )

    target_link_libraries(test_${testname} PRIVATE
            MyLib
            Catch2::Catch2WithMain
    )

    add_test(NAME test_${testname}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_${testname}
    )

endfunction(create_test)

Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.3.2
)
FetchContent_MakeAvailable(Catch2)

create_test("TcpServer")
