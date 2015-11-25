cmake_minimum_required(VERSION 3.3)
project(assignment_4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        cutest-1.5/AllTests.c
        cutest-1.5/CuTest.c
        cutest-1.5/CuTest.h
        cutest-1.5/CuTestTest.c
        src/main.c src/malloc/memory_allocation.c src/malloc/memory_allocation.h src/malloc/structs/memory.h src/malloc/utils/malloc_helpers.h src/malloc/utils/constants.h src/malloc/structs/free_block.h src/malloc/structs/free_block_list.h src/malloc/structs/unfree_block.h src/malloc/structs/tag.h src/test/test_memory_allocation.h src/test/utils/test_malloc_helpers.h src/test/structs/test_free_blocks.h src/test/structs/test_free_block_list.h src/test/structs/test_unfree_block.h src/test/test_schema.h)

add_executable(assignment_4 ${SOURCE_FILES})