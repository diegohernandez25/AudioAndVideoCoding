cmake_minimum_required(VERSION 3.15)
project(predictor)

set(CMAKE_CXX_STANDARD 11)
include_directories(Predictor)
set(SOURCE_FILES main.cpp Predictor/Predictor.cpp Predictor/Predictor.h)
add_executable(predictor ${SOURCE_FILES})