cmake_minimum_required(VERSION 3.26)
project(BlueScript)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")

set(PRECOMPILED_SOURCE ${CMAKE_SOURCE_DIR}/include/external/exprtk.hpp)

add_subdirectory(src)

add_executable(BlueScript main.cpp)
target_link_libraries(BlueScript PRIVATE stringMath)
target_include_directories(BlueScript PUBLIC "${CMAKE_SOURCE_DIR}/include")