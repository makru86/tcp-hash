#pragma once

#include "xxhash.h"
#include <cassert>
#include <functional>
#include <mutex>
#include <ostream>

namespace libtcp_hash {

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
  auto separatorFound{pos < std::string_view::npos};
  if (separatorFound) {
    onChunkCb(input.substr(0, pos), separatorFound);
    pos += 1; //< skip separator
    return tokenizer(input.substr(pos, std::string::npos), onChunkCb);
  }
  onChunkCb(input, separatorFound);
}

/* XxHash:
 * Compute hashsum of an input data of variable length, using xxHash XXH64 hash
 * function.
 *
 * Thread safety: None
 * Due to XXH64_ implementation, this class is thread-unsafe and must be created
 * and used on same thread only.
 */
class XxHash {
  XXH64_state_t *xxCtx_;
  XXH64_hash_t xxHash_;
  static constexpr size_t seed_ = 0;
std::mutex m;
public:
  explicit XxHash() noexcept(false);
  ~XxHash();
  XxHash(XxHash const&) = delete;
  XxHash &operator=(XxHash const&) = delete;

      // Add data to hash function
      XxHash &feed(CharArray chunk);

  // Return final hash, and prepare for next.
  HashValue digest();

private:
  void init();
  void deinit();
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
std::ostream &operator<<(std::ostream &os, FsmState state);

/*
 * FSM:
 * Adaptor on XxHash to work according to FSM rules.
 */
template <typename XxHashT> class StatefulHasher {
  XxHashT &xxHash_;
  FsmState state_ = FsmState::Empty;

public:
  explicit StatefulHasher(XxHashT &xxHash) : xxHash_(xxHash) {}

  StatefulHasher &feed(CharArray chunk) {
    if (chunk.empty()) {
      // Skip empty chunk.
      return *this;
    }
    switch (state_) {
    case FsmState::Empty:
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
      return 0; //< Ignore empty chunk.
    case FsmState::Accumulating:
    default:
      state_ = FsmState::Empty;
      return xxHash_.digest();
    };
  }

  FsmState state() const { return state_; };
};

} // namespace libtcp_hash