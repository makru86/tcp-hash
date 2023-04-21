# Find Boost asio
find_package(Boost REQUIRED COMPONENTS system)

# Find Google Benchmark
find_package(benchmark REQUIRED)

add_executable(loadtest loadtest.cpp TcpClient.h)
target_link_libraries(loadtest PRIVATE
        benchmark::benchmark
        benchmark::benchmark_main
        ${Boost_LIBRARIES}
        )
