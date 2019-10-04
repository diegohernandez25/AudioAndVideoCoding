cmake_minimum_required(VERSION 3.13)
project(test)

set(CMAKE_CXX_STANDARD 11)

add_executable(test main.cpp Wav/Wav.cpp Wav/Wav.h)