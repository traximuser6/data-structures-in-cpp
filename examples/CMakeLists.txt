cmake_minimum_required(VERSION 3.16)  # ← this works with CLion 2020.1

file(GLOB SAMPLE_SOURCES "*.cpp")

foreach (s_file ${SAMPLE_SOURCES})
    get_filename_component(s_name ${s_file} NAME_WE)
    add_executable(${s_name} ${s_file})
endforeach ()


