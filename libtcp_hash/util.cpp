#include <libtcp_hash/util.hpp>
#include <random>

namespace libtcp_hash {

uint64_t nanoSinceEpoch() {
  using namespace std::chrono;
  auto now = high_resolution_clock::now().time_since_epoch();
  return duration_cast<nanoseconds>(now).count();
}
std::string fixedFilledWidth(std::string const &text, size_t width,
                             std::string::value_type fill) {

  if (text.size() > width) {
    return text.substr(text.size() - width, width);
  }
  return std::string(width - text.size(), fill) + text;
}
std::string getEnvOrValue(const std::string &envName,
                          const std::string &orValue) {
  const char *envValue = std::getenv(envName.c_str());
  if (envValue) {
    return std::string(envValue);
  }
  return orValue;
}
int getEnvOrValue(const std::string &envName, int orValue) {
  return std::stoi(getEnvOrValue(envName, std::to_string(orValue)));
}
bool approximatelyEqual(size_t left, size_t rigth, size_t delta) {
  return (left >= rigth && left - rigth <= delta) ||
         (rigth > left && rigth - left <= delta);
}
std::string hexen(uint64_t value) {
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(16) << std::hex << value;
  return ss.str();
}
std::string readWholeFile(std::string_view filename) {
  std::ifstream file{filename.data()};
  std::string str((std::istreambuf_iterator<char>(file)),
                  std::istreambuf_iterator<char>());
  return str;
}

MetricsAnalized analyzeMetrics(const LoadtestMetrics &metrics) {
  MetricsAnalized analized{};
  analized.seconds = (metrics.timestampStop - metrics.timestampStart) / 1e9;
  analized.megaHashesPerSecond = //
      metrics.messagesReceived.load() / 1e6 / analized.seconds;
  analized.avgDataSize =
      1.0 * metrics.bytesSent.load() / metrics.messagesReceived.load();
  // avgDataSize:
  //    For a uniformly-distributed char random number generator that produces
  //    values in the range [0, 255], the average input line size in the limit
  //    tends to 255 B.
  return analized;
}

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
            << "\n Xonnections count:   " << value.clientSessions
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