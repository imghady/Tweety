cmake_minimum_required(VERSION 3.17)
project(server C)

set(CMAKE_C_STANDARD 11)

add_executable(server main.c cJSON.c)

target_link_libraries(server ws2_32)