cmake_minimum_required(VERSION 3.1.0)
set (CMAKE_CXX_STANDARD 11)
project(orderbook)
add_executable(orderbook main.cpp orderbook.cpp)
#set_target_properties(orderbook PROPERTIES
#CXX_STANDARD 17
#CXX_STANDARD_REQUIRED ON
#CXX_EXTENSIONS ON
#)
