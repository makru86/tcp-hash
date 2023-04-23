include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(MyLib STATIC
        TcpServer.cpp
        SignalHandler.cpp
        TcpHashClient.h
        TcpServer.h
        Log.h
        SignalHandler.h
        Hash.cpp
        Hash.h
        )

target_link_libraries(MyLib PUBLIC
        coverage_config
        Boost::system
        Threads::Threads
        xxHash::xxHash
        )