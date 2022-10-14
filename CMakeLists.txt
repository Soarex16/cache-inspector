cmake_minimum_required(VERSION 3.21)
project(l0_cache_inspector)

set(CMAKE_CXX_STANDARD 14)

add_executable(l0_cache_inspector driver.cpp chain_generator.hpp time_buffer.hpp detectors.hpp constants.hpp timer.hpp)