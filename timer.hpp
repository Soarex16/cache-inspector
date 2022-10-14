#pragma once

#include <cstddef>
#include <chrono>

#include "constants.hpp"
#include "chain_generator.hpp"

volatile void* global_p; // prevent caching in register
long long traverse_chain(void** chain, std::size_t num_steps) {
    void** p = (void**) chain;
    auto start = std::chrono::high_resolution_clock::now();
    auto c = num_steps;
    while (c-- > 0) {
        p = (void**) *p;
    }
    auto end = std::chrono::high_resolution_clock::now();
    global_p = *p;
    return (end - start).count();
}

long long run_epoch(const chain_generator& chain_gen, std::size_t memory_size, std::size_t stride, std::size_t num_repeats) {
    long long total_time = 0;
    for (std::size_t i = 0; i < num_repeats; ++i) {
        void** chain = chain_gen(memory_size, stride);

        auto time = traverse_chain(chain, read_count);
        total_time += time;

        delete[] chain;
    }
    return total_time / num_repeats;
}