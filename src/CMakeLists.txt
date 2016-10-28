cmake_minimum_required(VERSION 3.6)
project (space)

#
# Build system variables
#

#Sets c++11 flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#Workaround for bug on some ubuntu systems where CMake does not search for anything above Python 3.3
set(Python_ADDITIONAL_VERSIONS 2.7)

#
# Preprocessor macro's and variables
#

# Sets a preprocessor variable to enable os specific code to check the current build target
if(UNIX)
    add_definitions(-DTARGET_OS_UNIX_LIKE=1)
elseif(WIN32)
    add_definitions(-DTARGET_OS_WINDOWS=1)
endif(UNIX)

#
# Importing Boost
#
find_package(Boost 1.54.0 REQUIRED python)
add_library(boost SHARED IMPORTED)
set_property(TARGET boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})

add_library(boost-python SHARED IMPORTED)
set_property(TARGET boost-python PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
set_property(TARGET boost-python PROPERTY IMPORTED_LOCATION ${Boost_PYTHON_LIBRARY})

#
# Importing Python
#
find_package(PythonLibs 2.7 REQUIRED)
add_library(python SHARED IMPORTED)
set_property(TARGET python PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PYTHON_INCLUDE_DIRS})
set_property(TARGET python PROPERTY IMPORTED_LOCATION ${PYTHON_LIBRARIES})

#
# Print dependencies
#
#message(STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
#message(STATUS "PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE}")
#message(STATUS "PYTHON_INCLUDE_DIRS = ${PYTHON_INCLUDE_DIRS}")
#message(STATUS "BOOST LIBRARIES = ${Boost_LIBRARIES}")

#
# Build property file parser
#
add_subdirectory(property)

include_directories(${PYTHON_INCLUDE_DIRS})


add_executable(space Script.cpp Metrics.cpp Object.cpp Orbit.cpp Name.cpp main.cpp)
target_link_libraries(space property python boost-python)
