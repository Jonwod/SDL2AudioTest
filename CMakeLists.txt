cmake_minimum_required(VERSION 3.5)
project(test_audio)

set(CMAKE_CXX_STANDARD 14)

add_executable(test_audio main.cpp)
target_link_libraries(test_audio libSDL2.so)
