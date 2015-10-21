cmake_minimum_required(VERSION 3.3)
project(assignment_3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/disk_emu.c
    src/disk_emu.h
    src/fuse_wrappers.c
    src/sfs_test.c
    src/sfs_test2.c src/sfs_api.h)

add_executable(assignment_3 ${SOURCE_FILES})