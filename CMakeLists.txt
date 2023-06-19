cmake_minimum_required(VERSION 3.20)
project(SKIRMISH_GAME)

set(CMAKE_CXX_STANDARD 20)

add_executable(SKIRMISH_GAME main.cpp Unit.h Coord.h State.h State.cpp Turn.h Turn.cpp)
