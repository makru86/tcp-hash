#include "hash.hpp"

libtcp_hash::XxHash::XxHash() noexcept(false)
    : xxCtx_(nullptr), xxHash_(seed_) {
  xxCtx_ = XXH64_createState();
  if (xxCtx_ == nullptr) {
    throw std::runtime_error("XXH64_createState() failed");
  }
  reset();
}

void libtcp_hash::XxHash::reset() {

  if (XXH64_reset(xxCtx_, seed_) == XXH_ERROR) {
    throw std::runtime_error("XXH64_reset() failed");
  }
}

libtcp_hash::XxHash::~XxHash() { XXH64_freeState(xxCtx_); }

libtcp_hash::XxHash &libtcp_hash::XxHash::feed(libtcp_hash::StrView chunk) {
  if (XXH64_update(xxCtx_, chunk.data(), chunk.size()) == XXH_ERROR) {
    throw std::runtime_error("XXH64_update() failed");
  }
  return *this;
}
libtcp_hash::HashValue libtcp_hash::XxHash::digest() {
  XXH64_hash_t const hash = XXH64_digest(xxCtx_);

  // Reset for next run.
  reset();
  return hash;
}

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
