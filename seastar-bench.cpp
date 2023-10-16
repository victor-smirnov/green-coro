
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

    seastar::future<void> dig_into_fut(size_t depth = 0)
    {
        if (counter_ < counter_max_)
        {
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

    seastar::future<void> dig_into_coro(size_t depth = 0)
    {
        if (counter_ < counter_max_)
        {
            if (depth < 64) {
                counter_++;
                if (counter_ < counter_max_) {
                    co_await dig_into_coro(depth + 1);
                    co_await dig_into_coro(depth + 1);
                }
            }
        }
    }

    void dig_into_fibr(size_t depth = 0)
    {
        if (counter_ < counter_max_)
        {
            if (depth < 64) {
                counter_++;
                if (counter_ < counter_max_) {
                    dig_into_fibr(depth + 1);
                    dig_into_fibr(depth + 1);
                }
            }
        }

        if (++fib_counter_ > 255) {
            seastar::thread::yield();
            fib_counter_ = {};
        }
    }
};



int main(int argc, char** argv)
{
    seastar::app_template::seastar_options opts;
    opts.smp_opts.smp.set_value(1);

    seastar::app_template app(std::move(opts));
    app.run(argc, argv, [] () -> seastar::future<> {
        std::cout << "Hello world\n";

        TreeWalker walker(1ull << 26);

        long t0 = time_in_millis();
        co_await walker.dig_into_fut();
        long t1 = time_in_millis();

        std::cout << "Counter futu: " << walker.counter() << " :: " << (t1 - t0) << std::endl;
        walker.reset();
        co_await walker.dig_into_coro();
        long t2 = time_in_millis();

        std::cout << "Counter coro: " << walker.counter() << " :: " << (t2 - t1) << std::endl;


        walker.reset();
        co_await seastar::async([&]{
            walker.dig_into_fibr();
        });
        long t3 = time_in_millis();

        std::cout << "Counter fibr: " << walker.counter() << " :: " << (t3 - t2) << std::endl;

        walker.reset();
        walker.dig_into_natv();
        long t4 = time_in_millis();

        std::cout << "Counter natv: " << walker.counter() << " :: " << (t4 - t3) << std::endl;

    });

    return 0;
}
