include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCE_FILES
        TcpServer.cpp
        SignalHandler.cpp
)

set (MOC_HEADERS
)

set(HEADER_FILES
        TcpServer.h
        Log.h
        SignalHandler.h
)

add_library(MyLib STATIC
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

target_link_libraries(MyLib PUBLIC
        coverage_config
        Boost::system
        Threads::Threads
)