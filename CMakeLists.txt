cmake_minimum_required(VERSION 3.5.0)

project(Buffer VERSION 1.0 LANGUAGES CXX)

include_directories("${CMAKE_SOURCE_DIR}")

add_executable(buffer_test main.cpp buffer.h)