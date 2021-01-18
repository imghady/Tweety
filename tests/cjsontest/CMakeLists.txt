cmake_minimum_required(VERSION 3.17)
project(cjsontest C)

set(CMAKE_C_STANDARD 11)

add_executable(cjsontest main.c)

add_executable(cjsontest main.c cJSON.h cJSON.c)