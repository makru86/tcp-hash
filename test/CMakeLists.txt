function(create_test testname FILES)

    add_executable(test_${testname}
        main.cpp
        test_${testname}.cpp
        ${FILES}
    )

    target_link_libraries(test_${testname}
        PUBLIC
            ${Boost_LIBRARIES}
            coverage_config
    )

    add_test(NAME test_${testname}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_${testname}
    )

endfunction(create_test)

set(SOURCE_FILES
    ../src/lib/MyClass
    ../src/lib/MyClass.cpp
)


# find Boost package
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

create_test("MyClass" "${SOURCE_FILES}")
