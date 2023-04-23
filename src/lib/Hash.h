#pragma once

#include "xxhash.h"
#include <cassert>
#include <functional>
#include <ostream>

namespace app {

using CharArray = std::string_view;
using Char = CharArray::value_type;
using HashValue = uint64_t;

/*
 * tokenizer():
 * Convert input array of characters into an array of tokens, by
 * separating into chunks by '\n'. Chunk is a sequence of any characters
 * except the '\n', including empty sequence.
 *
 * Grammar:
 *  chunk = *ANY_CHAR_BUT_NEWLINE
 *  chunks = chunk [separator chunk]
 *  separator = NEWLINE
 *  NEWLINE = %x0A
 *  ANY_CHAR_BUT_NEWLINE = %x00-09 / %x0B-FF
 *
 * OnChunkCb: void(StringView chunk, bool newlineFound)
 */
template <typename OnChunkCb>
void tokenizer(CharArray input, OnChunkCb onChunkCb) {
  static constexpr Char separator_ = '\n';

  if (input.empty()) {
    return;
  }

  auto pos = input.find(separator_, 0);
  if (pos == std::string_view::npos) {
    // Separator not found - whole input is a chunk.
    onChunkCb(input, /* separatorFound: */ false);
    return;
  }
  // Separator found at pos:
  // - input before pos is a chunk
  onChunkCb(input.substr(0, pos), /* separatorFound: */ true);

  // - recursively call tokenizer() with the rest of the input.
  return tokenizer(input.substr(pos + 1, std::string::npos), onChunkCb);
}

/*
 * XxHash:
 * Compute hashsum of an input data of variable length, using xxHash XXH64 hash
 * function.
 */
class XxHash {

  XXH64_state_t *xxCtx_;
  XXH64_hash_t xxHash_;

  static constexpr size_t seed_ = 0;

public:
  explicit XxHash() : xxCtx_(nullptr), xxHash_(seed_) { init(); }

  ~XxHash() { deinit(); }

  XxHash &feed(CharArray chunk) {
    if (XXH64_update(xxCtx_, chunk.data(), chunk.size()) == XXH_ERROR) {
      throw std::runtime_error("XXH64_update() failed");
    }
    return *this;
  }

  HashValue digest() {
    XXH64_hash_t const hash = XXH64_digest(xxCtx_);

    // Self restart.
    deinit();
    init();
    return hash;
  }

private:
  void init() {
    xxCtx_ = XXH64_createState();
    if (xxCtx_ == nullptr) {
      throw std::runtime_error("XXH64_createState() failed");
    }
    if (XXH64_reset(xxCtx_, seed_) == XXH_ERROR) {
      throw std::runtime_error("XXH64_reset() failed");
    }
  }
  void deinit() { XXH64_freeState(xxCtx_); }
};

/*
 * FSM:
 *
 * State transition diagram:
 *  [*] -> Empty
 *  Empty -> Accumulating         : feed()
 *  Accumulating -> Accumulating  : feed()
 *  Accumulating -> Empty         : digest()
 *  Empty -> Empty                : feed(empty chunk)
 */
enum class FsmState {
  Empty,
  Accumulating,
};
std::ostream &operator<<(std::ostream &os, FsmState state) {
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

template <typename XxHashT> class FSM {

  XxHashT &xxHash_;
  FsmState state_ = FsmState::Empty;

public:
  explicit FSM(XxHashT &xxHash) : xxHash_(xxHash) {}

  FSM &feed(CharArray chunk) {
    if (chunk.empty()) {
      // Ignore empty chunk.
      return *this;
    }
    switch (state_) {
    case FsmState::Empty:
      // Start accumulating.
      state_ = FsmState::Accumulating;
      xxHash_.feed(chunk);
      return *this;
    case FsmState::Accumulating:
    default:
      xxHash_.feed(chunk);
      return *this;
    };
  }

  HashValue digest() {
    switch (state_) {
    case FsmState::Empty:
      // Ignore empty chunk.
      return 0;
    case FsmState::Accumulating:
    default:
      // Stop accumulating.
      state_ = FsmState::Empty;
      // Process accumulated chunk.
      return xxHash_.digest();
    };
  }

  FsmState state() const { return state_; };
};

} // namespace app