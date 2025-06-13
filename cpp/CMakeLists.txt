cmake_minimum_required(VERSION 3.7)
project(network_sim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(pybind11 REQUIRED)

pybind11_add_module(graph_module src/graph.cpp src/bindings.cpp)
target_include_directories(graph_module PRIVATE include)

pybind11_add_module(crc_module src/crc.cpp src/crc_bindings.cpp)
target_include_directories(crc_module PRIVATE include)

pybind11_add_module(stop_and_wait_module src/stop_and_wait.cpp src/stop_and_wait_bindings.cpp)
target_include_directories(stop_and_wait_module PRIVATE include)

pybind11_add_module(tcp_tahoe_module src/tcp_tahoe.cpp src/tcp_tahoe_bindings.cpp)
target_include_directories(tcp_tahoe_module PRIVATE include)
