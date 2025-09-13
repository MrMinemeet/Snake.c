cmake_minimum_required(VERSION 4.0)
project(Snake_c C)

set(CMAKE_C_STANDARD 23)

add_executable(Snake_c
        src/main.c
        src/util.c
        src/util.h
        src/GameState.c
        src/GameState.h
)
