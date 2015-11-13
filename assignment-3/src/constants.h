//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_CONSTANTS_H
#define ASSIGNMENT_3_CONSTANTS_H

#define FILE_NAME_LENGTH 32
#define FILE_EXTENSION_LENGTH 3

#define I_NODE_COUNT 100
#define FILE_DESCRIPTOR_TABLE_SIZE 8
#define DISK_BLOCK_CACHE_SIZE 64

#define BLOCKS_PER_I_NODE 11

// Constants related to disk stuff
#define SB_MAGIC 0xAABB0005
#define DISK_BLOCK_SIZE 512
#define DISK_BLOCK_COUNT 2048
#define FILE_SYSTEM_NAME "tmp4.disk"

// Disk indices of structures
#define SUPER_BLOCK_INDEX 0
#define I_NODE_TABLE_BLOCK_INDEX 1

#define DATA_BLOCK_TABLE_INDEX 101 // I_NODE_TABLE_BLOCK_INDEX + I_NODE_COUNT


#define DIRECTORIES_BLOCK_INDEX 32
#define FREE_BITMAP_BLOCK_INDEX 2000 // If this isn't high enough, then it overwrites data blocks!!!

/*
 * Dynamic
 */
static int const INDIRECT_BLOCK_POINTER_SIZE = DISK_BLOCK_SIZE / sizeof(int);

int bytesToBlockCount(size_t bytes) {
    int output = bytes / DISK_BLOCK_SIZE;
    if (bytes % DISK_BLOCK_SIZE != 0) {
        output++;
    }
    return output;
}

#endif //ASSIGNMENT_3_CONSTANTS_H
