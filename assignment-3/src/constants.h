//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_CONSTANTS_H
#define ASSIGNMENT_3_CONSTANTS_H

#include <stddef.h>

#define FILE_NAME_LENGTH 16
#define TOTAL_FILE_NAME_LENGTH 20
#define FILE_EXTENSION_LENGTH 3

#define I_NODE_COUNT 100
#define FILE_DESCRIPTOR_TABLE_SIZE 16

#define BLOCKS_PER_I_NODE 12

// Constants related to disk stuff
#define SB_MAGIC 0xAABB0005
#define DISK_BLOCK_SIZE 512
#define DISK_BLOCK_COUNT 2048
#define FILE_SYSTEM_NAME "fogarty.disk"

// Disk indices of structures
#define SUPER_BLOCK_INDEX 0 // 1 block large
#define I_NODE_TABLE_BLOCK_INDEX 1 // 16 blocks large
#define DIRECTORIES_BLOCK_INDEX 17 // 8 blocks large
#define FREE_BITMAP_BLOCK_INDEX 25 // 4 bytes large
#define DATA_BLOCK_TABLE_INDEX 30



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
