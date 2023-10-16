
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
public:
    TreeWalker(uint64_t max): TreeWalkerBase(max) {}

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
            boost::this_fiber::yield();
            fib_counter_ = {};
        }
    }
};


int main(int argc, char** argv)
{
    TreeWalker walker(1ull << 26);
    long t2 = time_in_millis();

    walker.reset();

    boost::fibers::fiber f0([&](){
        walker.dig_into_fibr();
    });

    f0.join();

    long t3 = time_in_millis();

    std::cout << "Counter fibr: " << walker.counter() << " :: " << (t3 - t2) << std::endl;
    return 0;
}
