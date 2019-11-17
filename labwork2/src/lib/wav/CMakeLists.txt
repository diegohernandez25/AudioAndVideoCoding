cmake_minimum_required(VERSION 3.15)
project(wav)

set(CMAKE_CXX_STANDARD 11)

add_executable(wav main.cpp wav.cpp wav.h)