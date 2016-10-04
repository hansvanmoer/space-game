cmake_minimum_required(VERSION 2.6)
project (space)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(UNIX)
    add_definitions(-DTARGET_OS_UNIX_LIKE=1)
elseif(WIN32)
    add_definitions(-DTARGET_OS_WINDOWS=1)
endif(UNIX)

find_package(Boost 1.54.0)

add_executable(space Metrics.cpp Object.cpp Orbit.cpp main.cpp)
