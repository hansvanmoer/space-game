#
# CMake configuration file for the IO library
#
cmake_minimum_required (VERSION 2.6)

project (space-io)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(space-io IOError.cpp)

