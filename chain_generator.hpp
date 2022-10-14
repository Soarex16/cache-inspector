#pragma once

#include <cstddef>
#include <functional>
#include <algorithm>
#include <random>

using chain_generator = std::function<void **(std::size_t size, std::size_t stride)>;

void **linear_reverse_pointer_chain_generator(std::size_t size, std::size_t stride = sizeof(void *)) {
    long long len = size / sizeof(void *);
    long long step = stride / sizeof(void *);
    void **memory = new void *[len];

    long long ptr = 0;
    while (ptr + step < len) {
        ptr += step;
        memory[ptr] = (void *) &memory[ptr - step];
    }
    memory[0] = (void *) &memory[ptr];
    return memory;
}

unsigned int draw(std::mt19937 engine, unsigned int upper_limit) {
    return std::uniform_int_distribution<unsigned int>(0, upper_limit - 1)(engine);
}

/* create a cyclic pointer chain that covers all words
   in a memory section of the given size in a randomized order */
void **randomized_pointer_chain_generator(std::size_t size, std::size_t stride = sizeof(void *)) {
    std::size_t len = size / sizeof(void *);
    std::size_t step = stride / sizeof(void *);
    void **memory = new void *[len];

    std::random_device rd;
    std::mt19937 engine(rd());

    // sample indices 0, step, 2 * step
    std::size_t start = 0;
    std::vector<std::size_t> indices(len);
    std::generate_n(
            std::back_inserter(indices),
            len / step,
            [&start, step]() {
                auto ret = start;
                start += step;
                return ret;
            }
    );

    // shuffle indices
    std::shuffle(indices.begin(), indices.end(), engine);

    // fill memory with pointer references
    for (std::size_t i = 1; i < len; ++i) {
        memory[indices[i - 1]] = (void *) &memory[indices[i]];
    }
    memory[indices[len - 1]] = (void *) &memory[indices[0]];
    return memory;
}