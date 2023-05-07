#include <boost/test/unit_test.hpp>
#include <iomanip>
#include <libtcp_hash/hash.hpp>
#include <libtcp_hash/util.hpp>
#include <list>

namespace libtcp_hash {

struct OnChunkCbArgs {
  StrView charArray_;
  bool separatorFound_;
};
bool operator==(const OnChunkCbArgs &left, const OnChunkCbArgs &right) {
  auto order = [](const OnChunkCbArgs &value) {
    return std::tie(value.charArray_, value.separatorFound_);
  };
  return order(left) == order(right);
}
std::ostream &operator<<(std::ostream &os, const OnChunkCbArgs &value) {
  os << "StrView: " << value.charArray_
     << ", separatorFound: " << value.separatorFound_;
  return os;
}

BOOST_AUTO_TEST_SUITE(HashTests)

BOOST_AUTO_TEST_CASE(TokenizerTest) {
  StrView input{
      "Hello, world!\n" //
      "\n"              //
      "abc\n"           //
      "foo bar"         //
  };

  std::list<OnChunkCbArgs> expected{
      {"Hello, world!", true}, //
      {"", true},              //
      {"abc", true},           //
      {"foo bar", false},      //
  };

  // Pass input to tokenizer, check that callback is called with expected
  // CallbackArgs:
  tokenizer(input, //
            [&](StrView chunk, bool separatorFound) {
              OnChunkCbArgs actual{chunk, separatorFound};
              BOOST_TEST(actual == expected.front());
              expected.pop_front();
            });
}

BOOST_AUTO_TEST_CASE(XxHashTest) {
  XxHash xxHash;

  // Same input - same result:
  BOOST_TEST(to_hex_str(xxHash.feed("Hello").digest()) == "0a75a91375b27d44");
  BOOST_TEST(to_hex_str(xxHash.feed("Hello").digest()) == "0a75a91375b27d44");

  // Empty input:
  BOOST_TEST(to_hex_str(xxHash.digest()) == "ef46db3751d8e999");
  BOOST_TEST(to_hex_str(xxHash.feed("").digest()) == "ef46db3751d8e999");
}

class MockXxHash {
public:
  StrView feedArg_;
  HashValue digestReturn_;

  MockXxHash() noexcept = default;
  ~MockXxHash() = default;
  MockXxHash(const MockXxHash &) = delete;
  MockXxHash &operator=(const MockXxHash &) = delete;

  MockXxHash &feed(StrView chunk) {
    BOOST_TEST(chunk == feedArg_);
    return *this;
  }
  HashValue digest() const { return digestReturn_; }
};

BOOST_AUTO_TEST_CASE(FsmTest) {
  using FSM = libtcp_hash::StatefulHasher<MockXxHash>;

  MockXxHash mockXxHash;
  FSM fsm{mockXxHash};
  BOOST_TEST(fsm.state() == FsmState::Empty);

  // Expect that FSM calls feed() and digest() on XxHash:
  mockXxHash.feedArg_ = "Hello";
  mockXxHash.digestReturn_ = 123;

  fsm.feed("Hello");
  BOOST_TEST(fsm.state() == FsmState::Accumulating);

  fsm.feed("Hello");
  BOOST_TEST(fsm.state() == FsmState::Accumulating);

  BOOST_TEST(fsm.digest() == 123);
  BOOST_TEST(fsm.state() == FsmState::Empty);

  fsm.feed("");
  BOOST_TEST(fsm.state() == FsmState::Empty);
}

BOOST_AUTO_TEST_CASE(FsmXxHashTokenizerTest) {
  using FSM = libtcp_hash::StatefulHasher<XxHash>;

  XxHash xxHash;
  FSM fsm{xxHash};

  StrView input{
      "Hello, world!\n" //
      "\n"              //
      "abc\n"           //
      "foo bar"         //
  };

  std::list<HashValue> expected{
      17691043854468224118ull, //
      0,                       // hash value of empty string is zero
      4952883123889572249ull,  //
                              // no 4th hash value, because newline was not sent
  };

  BOOST_TEST(to_hex_str(4952883123889572249ull) == "44bc2cf5ad770999");

  // Pass input to tokenizer, check that callback is called with expected
  // HashValue:
  tokenizer(input, //
            [&](StrView chunk, bool separatorFound) {
              fsm.feed(chunk);
              if (separatorFound) {
                HashValue actual = fsm.digest();
                BOOST_TEST(actual == expected.front());
                expected.pop_front();
              }
            });
}
/* TODO
// fast integer to hex string convertor
// https://stackoverflow.com/a/5100748/84283
class FastHexString {
  static constexpr auto buf_size_{16};
  std::array<char, buf_size_ + 1> buffer;
  char const *const digitsHex = "0123456789abcdef";

public:
  constexpr auto size() const { return buf_size_; }
  const char *operator()(uint64_t value) {
    auto p = buffer.end();
    do {
      *--p = digitsHex[value & 0xF];
      value >>= 4;
    } while (value);
    return p;
  }
};

BOOST_AUTO_TEST_CASE(FastHexStringTest) {
  FastHexString to_hex_str;

  BOOST_TEST(std::string("0000000000000000") == to_hex_str(0));
  BOOST_TEST(std::string("0000000000000001") == to_hex_str(1));
  BOOST_TEST(std::string("123456789abcdef0") == to_hex_str(0x123456789ABCDEF0));
  BOOST_TEST(std::string("0000000000000002") == to_hex_str(2));
}
*/

BOOST_AUTO_TEST_SUITE_END()

} // namespace libtcp_hash
