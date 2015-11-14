cmake_minimum_required(VERSION 3.3)
project(assignment_3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/data_structures/directory.h
    src/data_structures/directory_cache.h
    src/data_structures/disk_block_cache.h
    src/data_structures/file_descriptor.h
    src/data_structures/file_descriptor_table.h
    src/data_structures/free_bitmap.h
    src/data_structures/i_node.h
    src/data_structures/i_node_table.h
    src/data_structures/super_block.h
    src/constants.h
    src/disk_emu.c
    src/disk_emu.h
    src/fuse_wrappers.c
    src/fuse_wrappers.h
    src/sfs_api.c
    src/sfs_api.h
    src/sfs_test.c
    src/sfs_test2.c src/helpers/disk_access.h src/data_structures/indirect_block_pointer.h)

add_executable(assignment_3 ${SOURCE_FILES})