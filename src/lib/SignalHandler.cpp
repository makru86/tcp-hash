#include "SignalHandler.h"

namespace app {

std::condition_variable SignalHandler::cv_;
std::mutex SignalHandler::mutex_;
std::atomic<bool> SignalHandler::interrupted_(false);

} // namespace app