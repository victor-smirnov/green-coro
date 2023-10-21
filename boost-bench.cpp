
// Copyright 2023 Victor Smirnov
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include <boost/fiber/fiber.hpp>
#include <boost/fiber/operations.hpp>


#include "common.hpp"



using namespace gc;

class TreeWalker: public TreeWalkerBase {
    uint64_t fib_counter_{};
    uint64_t span_{};
public:
    TreeWalker(uint64_t max): TreeWalkerBase(max) {}

    void dig_into_fiber(size_t depth = 0) {
        if (counter_ < counter_max_) {
            if (depth < 64) {
                counter_++;
                if (counter_ < counter_max_) {
                    dig_into_fiber(depth + 1);
                    dig_into_fiber(depth + 1);
                }
            }
        }

        if (++fib_counter_ >= span_) {
            boost::this_fiber::yield();
            fib_counter_ = {};
        }
    }

    void set_span(uint64_t span) {
        span_ = span;
    }

    void reset() {
        counter_ = {};
        fib_counter_ = {};
    }
};


int main(int argc, char** argv)
{
    TreeWalker walker(1ull << 26);
    long t2 = time_in_millis();

    walker.reset();
    walker.set_span(1);
    boost::fibers::fiber f0([&](){
        walker.dig_into_fiber();
    });
    f0.join();

    long t3 = time_in_millis();

    std::cout << "(6) Boost Fibers (raw): " << (t3 - t2) << " ms, "
              << walker.speed(t3 - t2) << "M iters/sec" << std::endl;

    walker.reset();
    walker.set_span(256);
    boost::fibers::fiber f1([&](){
        walker.dig_into_fiber();
    });
    f1.join();

    long t4 = time_in_millis();

    std::cout << "(7) Boost Fibers (opt): " << (t4 - t3) << " ms, "
              << walker.speed(t4 - t3) << "M iters/sec" << std::endl;

    return 0;
}
