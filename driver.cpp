#include <iostream>

#include "detectors.hpp"

int main() {
    auto cache_stats = naive_cache_size_detector();
    std::cout <<
        "cache size\t" << cache_stats.cache_size << '\n' <<
        "associativity\t" << cache_stats.associativity << '\n' <<
        "cache line size\t" << cache_stats.cache_line_size << std::endl;

    // cache_size: 131072
    // cache_line_size: 128
    // associativity: presumably 8
    return 0;
}
