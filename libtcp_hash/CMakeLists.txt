include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(libtcp_hash STATIC
        Server.cpp
        Server.h
        Hash.cpp
        Hash.h
        Util.h
        )

target_link_libraries(libtcp_hash PUBLIC
        coverage_config
        Boost::system
        Threads::Threads
        xxHash::xxHash
        )

target_include_directories(libtcp_hash PUBLIC
        "${CMAKE_SOURCE_DIR}"
        )