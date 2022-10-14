#pragma once

const int KB = 1024;
const int MB = 1024 * KB;

const long read_count = 1'000'000;
const long repeats = 2;

const long min_cache_size = 1 * KB;
const long max_cache_size = 256 * KB;

const long min_assoc = 1;
const long max_assoc = 20;
const long min_stride = 16;
const long max_memory = 4 * MB;

const long trace_threshold = 256 * KB;