/*
 * loadtest.cpp
 * Performance testing for the tcp_hash application.
 *
 * Inspired by chronoxor/CppServer benchmark:
 * https://github.com/chronoxor/CppServer/blob/master/performance/tcp_echo_client.cpp
 *
>>> Server address: 127.0.0.1
>>> Server port: 1111
>>> Working clients: 100
>>> Working messages: 1000
>>> Message size: 32
>>> Seconds to benchmarking: 10
>>>
>>> Errors: 0
>>>
>>> Total time: 10.007 s
>>> Total data: 1.151 GiB
>>> Total messages: 38503396
>>> Data throughput: 117.423 MiB/s
>>> Message latency: 259 ns
>>> Message throughput: 3847402 msg/s
 */

// Google benchmark not used currently.
// BENCHMARK_MAIN();
void serverLoadtest();
void hashLoadtest();

int main() {
  hashLoadtest();
  serverLoadtest();
  return 0;
}