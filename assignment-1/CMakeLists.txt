cmake_minimum_required(VERSION 3.3)
project(assignment_1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    linkedlist.h
    oscode.c)

add_executable(assignment_1 ${SOURCE_FILES})