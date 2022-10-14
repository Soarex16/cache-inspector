#pragma once

#include <cstddef>
#include <algorithm>
#include <iostream>
#include <vector>
#include <set>

#include "timer.hpp"
#include "constants.hpp"
#include "time_buffer.hpp"
#include "chain_generator.hpp"

struct cache_properties {
    std::size_t cache_size = 0;
    std::size_t associativity = 0;
    std::size_t cache_line_size = 0; // still have troubles with cache line
};

/**
 * Simple benchmark to determine cache size
 */
cache_properties naive_cache_size_detector() {
    cache_properties props;

    time_buffer<std::size_t> buffer(10);
    for (std::size_t memory_size = min_cache_size; memory_size <= max_cache_size; memory_size += 1024) {
        long long epoch_time = run_epoch(randomized_pointer_chain_generator, memory_size, sizeof(void*), repeats);
        buffer.record((double)epoch_time, memory_size);

        std::cout << memory_size << "\t" << epoch_time << "\t" << buffer.mean_delta() << std::endl;

        if (buffer.mean_delta() > 0.03) {
            props = cache_properties {
                    .cache_size = buffer.get_rear_item()
            };
        }
    }

    return props;
}

std::vector<cache_properties> detect_entities(std::vector<std::set<std::size_t>>& jumps_traces, std::size_t stride) {
    std::vector<cache_properties> caches;
    std::set<std::size_t> stable_trace = jumps_traces[jumps_traces.size() - 1];
    std::reverse(jumps_traces.begin(), jumps_traces.end());

    for (const auto& trace: jumps_traces) {
        std::set<std::size_t> detected_entities;
        for (std::size_t associativity : stable_trace) {
            if (!trace.count(associativity)) {
                caches.push_back(cache_properties {
                        .cache_size = stride * associativity,
                        .associativity = associativity
                });
                detected_entities.insert(associativity);
            }
        }

        for (std::size_t s : detected_entities) {
            stable_trace.erase(s);
        }

        stride /= 2;
    }

    std::sort(caches.begin(), caches.end(), [](const cache_properties& a, const cache_properties& b) {
        return a.cache_size < b.cache_size;
    });

    return caches;
}

// https://etd.ohiolink.edu/apexprod/rws_etd/send_file/send?accession=osu1308256764
cache_properties robust_detector() {
    std::vector<std::set<std::size_t>> jumps_traces;
    std::size_t stride = min_stride;

    for (; stride * max_assoc < max_memory; stride *= 2) {
        long long prev_epoch_time = -1;
        std::set<std::size_t> current_trace;

        for (std::size_t assoc = min_assoc; assoc < max_assoc; ++assoc) {
            std::size_t size = stride * assoc;
            long long epoch_time = run_epoch(linear_reverse_pointer_chain_generator, size, stride, repeats);

            std::cout << "stride\t" << stride << "\tassoc\t" << assoc << "\ttime\t" << epoch_time << std::endl;

            if (prev_epoch_time != -1 && (double) (epoch_time - prev_epoch_time) / epoch_time > 0.1) {
                current_trace.insert(assoc - 1);
            }

            prev_epoch_time = epoch_time;
        }

        bool same = true;
        // compare current and prev jump records to determine shift
        if (!jumps_traces.empty()) {
            const auto& prev_trace = jumps_traces[jumps_traces.size() - 1];
            for (std::size_t jump : current_trace) {
                same &= prev_trace.count(jump);
            }
            for (std::size_t jump : prev_trace) {
                same &= current_trace.count(jump);
            }
        }

        if (same && stride >= trace_threshold) {
            break;
        }

        jumps_traces.push_back(current_trace);
    }

    std::vector<cache_properties> caches = detect_entities(jumps_traces, stride);
    return caches[0];
}