cmake_minimum_required(VERSION 2.6)
project (space)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(io)

add_executable(space main.cpp)
target_link_libraries(space PUBLIC space-io)
