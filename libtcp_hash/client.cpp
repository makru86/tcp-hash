#include <libtcp_hash/client.hpp>
#include <random>

namespace libtcp_hash {

std::string randomString(size_t length, unsigned int seed) {
  std::string result;
  result.resize(length);
  std::mt19937 gen(seed);
  std::uniform_int_distribution<> dis(0, 255);
  for (size_t i = 0; i < length; ++i) {
    result[i] = dis(gen);
  }
  return result;
}

std::ostream &operator<<(std::ostream &os, LoadtestConfig const &value) {
  return os << "LoadtestConfig:"
            << "\n Server TCP address:  " << value.tcpAddress
            << "\n Connections count:   " << value.clientSessions
            << "\n Data size:           " << value.dataSize
            << "\n Repeat count:        " << value.repeatDataIterations
            << "\n RNG seed:            " << value.randomGeneratorSeed << "\n";
}

std::ostream &operator<<(std::ostream &os, LoadtestMetrics const &value) {
  return os << "LoadtestMetrics:"                                         //
            << "\n Timestamp:"                                            //
            << "\n   start:       " << value.timestampStart << " us"      //
            << "\n   stop:        " << value.timestampStop << " us"       //
            << "\n Messages:"                                             //
            << "\n   sent:        " << value.messagesSent                 //
            << "\n   received:    " << value.messagesReceived             //
            << "\n Bytes:"                                                //
            << "\n   sent:       " << value.bytesSent / 1e6 << " MiB"     //
            << "\n   received:   " << value.bytesReceived / 1e6 << " MiB" //
            << "\n";
}

std::ostream &operator<<(std::ostream &os, MetricsAnalized const &value) {
  return os << "MetricsAnalized:"                              //
            << "\n Hashrate:"                                  //
            << "\n   " << value.megaHashesPerSecond << " MH/s" //
            << "\n Latency:"                                   //
            << "\n   TODO"                                     //
            << "\n Average data size:"                         //
            << "\n   " << value.avgDataSize << " B"            //
            << "\n Total time:"                                //
            << "\n   " << value.seconds << " s"                //
            << "\n";
}

} // namespace libtcp_hash