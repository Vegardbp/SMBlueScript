cmake_minimum_required(VERSION 3.26)
project(BlueScript)

set(CMAKE_CXX_STANDARD 17)

add_executable(BlueScript main.cpp)
target_include_directories(BlueScript PUBLIC "${CMAKE_SOURCE_DIR}/include")