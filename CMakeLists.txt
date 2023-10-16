cmake_minimum_required (VERSION 3.16)
project (GreenCoro)

add_library (ucontext::ucontext INTERFACE IMPORTED)
add_library (SystemTap::SDT INTERFACE IMPORTED)

find_package(Seastar REQUIRED)
find_package(Boost
    COMPONENTS
        context
        fiber
    REQUIRED
)

add_executable(seastar-bench seastar-bench.cpp)
target_link_libraries(seastar-bench PUBLIC Seastar::seastar)
target_compile_options(seastar-bench PUBLIC -fcoroutines -std=c++20)

add_executable(boost-bench boost-bench.cpp)
target_link_libraries(boost-bench PUBLIC Boost::context Boost::fiber)
