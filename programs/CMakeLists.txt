cmake_minimum_required(VERSION 3.17)
project(programs C)

set(CMAKE_C_STANDARD 11)

add_executable(programs main.c cJSON.c)

target_link_libraries(programs ws2_32)

