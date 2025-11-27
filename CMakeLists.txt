cmake_minimum_required(VERSION 3.16)  # ← this works with CLion 2020.1
project(data_structures_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(exercises)
add_subdirectory(samples)
add_subdirectory(examples)

add_subdirectory(projects/banking-app)
