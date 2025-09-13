cmake_minimum_required(VERSION 4.0)
project(Snake_c C)

set(CMAKE_C_STANDARD 23)

add_executable(Snake_c main.c
        util.c
        util.h
)
