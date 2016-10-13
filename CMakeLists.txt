# -*- cmake -*-
cmake_minimum_required(VERSION 2.6)
project(pvm)

add_definitions(-std=c++11)

add_subdirectory(bytecode)

add_executable(pvm main.cc)
target_link_libraries(pvm bytecode)
