#pragma once

#include <iostream>
#include <condition_variable>
#include <csignal>
#include <atomic>
#include <functional>

#include "Log.h"

namespace app {

    class SignalHandler {

        static std::condition_variable cv_;
        static std::mutex mutex_;
        static std::atomic<bool> interrupted_;

    public:
        SignalHandler() {
            interrupted_ = false;
            // Set up signal handler
            std::signal(SIGINT, SignalHandler::signal_handler);
        }

        void wait_for_interrupt() {
            LOG_INFO("Press Ctrl-C to interrupt.");

            std::unique_lock<std::mutex> lk(mutex_);
            cv_.wait(lk, [this] { return interrupted_.load(); });

            // Ctrl-C was pressed
            LOG_INFO("Interrupted.");
        }

        [[nodiscard]] bool interrupted() const {
            return interrupted_.load();
        }

    private:
        static void signal_handler(int signal) {
            if (signal == SIGINT) {
                std::unique_lock<std::mutex> lk(mutex_);
                interrupted_.store(true);
                cv_.notify_one();
            }
        }
    };
}
