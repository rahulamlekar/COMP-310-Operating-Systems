cmake_minimum_required(VERSION 3.3)
project(assignment_2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/assignment-2.c
    src/authentication.h
    src/client.c
    src/fifo_buffer.h
    src/fifo_buffer_ops.c
    src/fifo_buffer_ops.h
    src/print_job.h
    src/shared_mem.h)

add_executable(assignment_2 ${SOURCE_FILES})