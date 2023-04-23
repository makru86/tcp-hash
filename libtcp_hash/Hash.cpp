#include "Hash.h"

app::XxHash::XxHash() : xxCtx_(nullptr), xxHash_(seed_) { init(); }
app::XxHash::~XxHash() { deinit(); }
app::XxHash &app::XxHash::feed(app::CharArray chunk) {
  if (XXH64_update(xxCtx_, chunk.data(), chunk.size()) == XXH_ERROR) {
    throw std::runtime_error("XXH64_update() failed");
  }
  return *this;
}
app::HashValue app::XxHash::digest() {
  XXH64_hash_t const hash = XXH64_digest(xxCtx_);

  // Self restart.
  deinit();
  init();
  return hash;
}
void app::XxHash::init() {
  xxCtx_ = XXH64_createState();
  if (xxCtx_ == nullptr) {
    throw std::runtime_error("XXH64_createState() failed");
  }
  if (XXH64_reset(xxCtx_, seed_) == XXH_ERROR) {
    throw std::runtime_error("XXH64_reset() failed");
  }
}
void app::XxHash::deinit() { XXH64_freeState(xxCtx_); }
std::ostream &app::operator<<(std::ostream &os, app::FsmState state) {
  switch (state) {
  case FsmState::Empty:
    os << "Empty";
    break;
  case FsmState::Accumulating:
    os << "Accumulating";
    break;
  default:
    os << "Unknown";
    break;
  };
  return os;
}
