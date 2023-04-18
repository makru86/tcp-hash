set(SOURCE_FILES
    main.cpp
)

set(HEADER_FILES
)

add_executable("${CMAKE_PROJECT_NAME}"
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

find_package(Boost REQUIRED COMPONENTS system)
target_link_libraries("${CMAKE_PROJECT_NAME}"
    PUBLIC
        MyLib
        Boost::system
)