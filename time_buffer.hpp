#pragma once

#include <cstddef>
#include <vector>

template<typename T>
class time_buffer {
public:
    time_buffer(std::size_t capacity) {
        buffer_.resize(capacity);
    }

    void record(double timestamp, T payload) {
        auto record = time_buffer_record {
                .timestamp = timestamp,
                .delta = 0,
                .payload = payload
        };
        if (items_ > 0) {
            const time_buffer_record& prev_record = buffer_[get_prev_ptr()];
            record.delta = record.timestamp - prev_record.timestamp;
        }
        buffer_[ptr_] = record;
        if (items_ < buffer_.size()) {
            ++items_;
        }
        ptr_ = get_next_ptr();
    }

    double mean_delta() {
        double timestamp_sum = 0;
        double delta_sum = 0;
        for (std::size_t i = 0; i < items_; ++i) {
            timestamp_sum += buffer_[i].timestamp;
            delta_sum += buffer_[i].delta;
        }

        return delta_sum / timestamp_sum;
    }

    T get_rear_item() {
        long long pos = ptr_ - items_ + 1;
        if (pos < 0) {
            pos = items_ + pos;
        }

        return buffer_[pos].payload;
    }

private:
    std::size_t get_next_ptr() {
        return (ptr_ + 1) % buffer_.size();
    }

    std::size_t get_prev_ptr() {
        if (ptr_ == 0) {
            return items_ - 1;
        } else {
            return ptr_ - 1;
        }
    }

    struct time_buffer_record {
        double timestamp = 0;
        double delta = 0;
        T payload;
    };

    std::vector<time_buffer_record> buffer_;
    std::size_t ptr_ = 0;
    std::size_t items_ = 0;
};