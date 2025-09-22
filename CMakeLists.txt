cmake_minimum_required(VERSION 3.28)
project(data_structures_cpp)

set(CMAKE_CXX_STANDARD 17) # Changed from 20 to 17 to avoid modules issue set(CMAKE_CXX_STANDARD_REQUIRED ON) set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 0 CACHE BOOL "" FORCE)

add_executable(data_structures_cpp main.cpp)