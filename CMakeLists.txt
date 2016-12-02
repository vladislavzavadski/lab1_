cmake_minimum_required(VERSION 3.6)
project(Laba1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Server.cpp Server.h Socket.cpp Socket.h)
add_executable(Laba1 ${SOURCE_FILES})