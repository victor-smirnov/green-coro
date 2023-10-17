
// Copyright 2023 Victor Smirnov
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "common.hpp"

#include <seastar/core/app-template.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/coroutine.hh>
#include <seastar/core/thread.hh>

#include <chrono>
#include <iostream>

using namespace gc;

class TreeWalker: public TreeWalkerBase {
public:
    TreeWalker(uint64_t max): TreeWalkerBase(max) {}

    seastar::future<void> dig_into_fut(size_t depth = 0) {
        if (counter_ < counter_max_) {
            if (depth < 64) {
                counter_++;
                if (counter_ < counter_max_) {
                    return dig_into_fut(depth + 1).then([this, depth]{
                        return dig_into_fut(depth + 1);
                    });
                }
                else {
                    return seastar::make_ready_future();
                }
            }
        }

        return seastar::make_ready_future();
    }

    seastar::future<void> dig_into_coro(size_t depth = 0) {
        if (counter_ < counter_max_) {
            if (depth < 64) {
                counter_++;
                if (counter_ < counter_max_) {
                    co_await dig_into_coro(depth + 1);
                    co_await dig_into_coro(depth + 1);
                }
            }
        }
    }

    void dig_into_fiber1(size_t depth = 0) {
        if (counter_ < counter_max_) {
            if (depth < 64) {
                counter_++;
                if (counter_ < counter_max_) {
                    dig_into_fiber1(depth + 1);
                    dig_into_fiber1(depth + 1);
                }
            }
        }

        seastar::thread::maybe_yield();
    }

    void dig_into_fiber2(size_t depth = 0) {
        if (counter_ < counter_max_) {
            if (depth < 64) {
                counter_++;
                if (counter_ < counter_max_) {
                    dig_into_fiber2(depth + 1);
                    dig_into_fiber2(depth + 1);
                }
            }
        }

        seastar::thread::yield();
    }
};



int main(int argc, char** argv)
{
    seastar::app_template::seastar_options opts;
    opts.smp_opts.smp.set_value(1);

    seastar::app_template app(std::move(opts));
    app.run(argc, argv, [] () -> seastar::future<> {
        TreeWalker walker(1ull << 26);

        long t0 = time_in_millis();
        co_await walker.dig_into_fut();
        long t1 = time_in_millis();

        std::cout << "(1) Futures:      " << (t1 - t0) << " ms, "
                  << walker.speed(t1 - t0) << "M iters/sec" << std::endl;

        walker.reset();
        co_await walker.dig_into_coro();
        long t2 = time_in_millis();
        std::cout << "(2) Coroutines:   " << (t2 - t1) << " ms, "
                  << walker.speed(t2 - t1) << "M iters/sec" << std::endl;

        walker.reset();
        co_await seastar::async([&]{
            walker.dig_into_fiber1();
        });
        long t3 = time_in_millis();

        std::cout << "(3) Fibers (opt): " << (t3 - t2) << " ms, "
                  << walker.speed(t3 - t2) << "M iters/sec" << std::endl;

        walker.reset();
        co_await seastar::async([&]{
            walker.dig_into_fiber2();
        });
        long t4 = time_in_millis();

        std::cout << "(4) Fibers (raw): " << (t4 - t3) << " ms, "
                  << walker.speed(t4 - t3) << "M iters/sec" << std::endl;


        walker.reset();
        walker.dig_into_natv();
        long t5 = time_in_millis();

        std::cout << "(5) Raw calls:    " << (t5 - t4) << " ms, "
                  << walker.speed(t5 - t4) << "M iters/sec" << std::endl;

    });

    return 0;
}
