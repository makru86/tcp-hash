#include <boost/test/unit_test.hpp>
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

std::string to_hex_str(uint64_t value) {
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

} // namespace libtcp_hash
