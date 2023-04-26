#include <boost/test/unit_test.hpp>
#include <iomanip>
#include <libtcp_hash/hash.h>
#include <libtcp_hash/util.h>
#include <list>

namespace libtcp_hash {

struct OnChunkCbArgs {
  CharArray charArray_;
  bool separatorFound_;
};
bool operator==(const OnChunkCbArgs &left, const OnChunkCbArgs &right) {
  auto order = [](const OnChunkCbArgs &value) {
    return std::tie(value.charArray_, value.separatorFound_);
  };
  return order(left) == order(right);
}
std::ostream &operator<<(std::ostream &os, const OnChunkCbArgs &value) {
  os << "CharArray: " << value.charArray_
     << ", separatorFound: " << value.separatorFound_;
  return os;
}

BOOST_AUTO_TEST_SUITE(HashTests)

BOOST_AUTO_TEST_CASE(TokenizerTest) {
  CharArray input{
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
            [&](CharArray chunk, bool separatorFound) {
              OnChunkCbArgs actual{chunk, separatorFound};
              BOOST_TEST(actual == expected.front());
              expected.pop_front();
            });
}

BOOST_AUTO_TEST_CASE(XxHashTest) {
  XxHash xxHash;

  // Same input - same result:
  BOOST_TEST(hexen(xxHash.feed("Hello").digest()) == "0a75a91375b27d44");
  BOOST_TEST(hexen(xxHash.feed("Hello").digest()) == "0a75a91375b27d44");

  // Empty input:
  BOOST_TEST(hexen(xxHash.digest()) == "ef46db3751d8e999");
  BOOST_TEST(hexen(xxHash.feed("").digest()) == "ef46db3751d8e999");
}

struct MockXxHash {
  CharArray feedArg_;
  HashValue digestReturn_;

  MockXxHash &feed(CharArray chunk) {
    BOOST_TEST(chunk == feedArg_);
    return *this;
  }

  HashValue digest() { return digestReturn_; }
};

BOOST_AUTO_TEST_CASE(FsmTest) {
  using FSM = libtcp_hash::FSM<MockXxHash>;

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
  using FSM = libtcp_hash::FSM<XxHash>;

  XxHash xxHash;
  FSM fsm{xxHash};

  CharArray input{
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

  // Pass input to tokenizer, check that callback is called with expected
  // HashValue:
  tokenizer(input, //
            [&](CharArray chunk, bool separatorFound) {
              fsm.feed(chunk);
              if (separatorFound) {
                HashValue actual = fsm.digest();
                BOOST_TEST(actual == expected.front());
                expected.pop_front();
              }
            });
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace libtcp_hash