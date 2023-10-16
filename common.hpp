

// Copyright 2023 Victor Smirnov
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once

#include <iostream>
#include <chrono>

namespace gc {


class TreeWalkerBase {
protected:
    uint64_t counter_{};
    const uint64_t counter_max_;

    uint64_t fib_counter_{};

public:
    TreeWalkerBase(uint64_t max): counter_max_(max) {}

    void dig_into_natv(size_t depth = 0)
    {
        if (counter_ < counter_max_)
        {
            if (depth < 64) {
                counter_++;
                if (counter_ < counter_max_) {
                    dig_into_natv(depth + 1);
                    dig_into_natv(depth + 1);
                }
            }
        }
    }

    void reset() {
        counter_ = {};
        fib_counter_ = {};
    }

    uint64_t counter() const {return counter_;}
};


static inline int64_t time_in_millis() {
    std::chrono::time_point<std::chrono::system_clock> now =
    std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

}
