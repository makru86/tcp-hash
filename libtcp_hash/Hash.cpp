#include "Hash.h"

libtcp_hash::XxHash::XxHash() : xxCtx_(nullptr), xxHash_(seed_) { init(); }
libtcp_hash::XxHash::~XxHash() { deinit(); }
libtcp_hash::XxHash &libtcp_hash::XxHash::feed(libtcp_hash::CharArray chunk) {
  if (XXH64_update(xxCtx_, chunk.data(), chunk.size()) == XXH_ERROR) {
    throw std::runtime_error("XXH64_update() failed");
  }
  return *this;
}
libtcp_hash::HashValue libtcp_hash::XxHash::digest() {
  XXH64_hash_t const hash = XXH64_digest(xxCtx_);

  // Self restart.
  deinit();
  init();
  return hash;
}
void libtcp_hash::XxHash::init() {
  xxCtx_ = XXH64_createState();
  if (xxCtx_ == nullptr) {
    throw std::runtime_error("XXH64_createState() failed");
  }
  if (XXH64_reset(xxCtx_, seed_) == XXH_ERROR) {
    throw std::runtime_error("XXH64_reset() failed");
  }
}
void libtcp_hash::XxHash::deinit() { XXH64_freeState(xxCtx_); }
std::ostream &libtcp_hash::operator<<(std::ostream &os,
                                      libtcp_hash::FsmState state) {
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
