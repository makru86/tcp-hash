#include <boost/test/unit_test.hpp>

#include <thread>

#include "../src/lib/SignalHandler.h"

using app::SignalHandler;

BOOST_AUTO_TEST_SUITE(SignalHandlerTests)

    BOOST_AUTO_TEST_CASE(InterruptedTest)
    {
        SignalHandler sh;
        std::raise(SIGINT);
        BOOST_CHECK(sh.interrupted());
    }

    BOOST_AUTO_TEST_CASE(WaitForInterruptTest)
    {
        SignalHandler sh;

        std::thread t([&sh]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::raise(SIGINT);
        });

        sh.wait_for_interrupt();
        BOOST_CHECK(sh.interrupted());

        t.join();
    }

BOOST_AUTO_TEST_SUITE_END()
