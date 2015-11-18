//
// Created by Andrew Fogarty on 2015-11-08.
//

#ifndef ASSIGNMENT_3_DISK_ACCESS_H
#define ASSIGNMENT_3_DISK_ACCESS_H

#include <stdlib.h>
#include <string.h>
#include "../data_structures/i_node.h"
#include "../constants.h"
#include "../disk_emu.h"
#include "../data_structures/free_bitmap.h"
#include "../data_structures/i_node_table.h"
#include "../data_structures/directory_cache.h"

void read_data_block(int index, void* buffer) {
    printf("read_data_block(%d)\n", index);
    read_blocks(DATA_BLOCK_TABLE_INDEX + index, 1, buffer);
}
void write_data_block(int index, void* buffer) {
    printf("write_data_block(%d)\n", index);
    write_blocks(DATA_BLOCK_TABLE_INDEX + index, 1, buffer);
}
/**
 * Fill a disk block with empty space.
 */
void erase_data_block(int index) {
    void* empty = malloc(DISK_BLOCK_SIZE);
    memset(empty, '\0', DISK_BLOCK_SIZE);
    write_data_block(index, empty);
    free(empty);
}

/**
 * Write arbitrarily large data to the disk from a particulal index
 */
void write_data_blocks(void* data, int startingIndex, size_t size) {
    int numBlocks = bytesToBlockCount(size);
    //printf("Writing %d bytes to %d blocks.\n", size, numBlocks);
    // Make a blank copy of the correct number of blocks
    void* dataGoingToDisk = malloc((size_t) numBlocks * DISK_BLOCK_SIZE);
    memset(dataGoingToDisk, '\0', (size_t) numBlocks * DISK_BLOCK_SIZE);
    // Copy the incoming data to the temp copy
    memcpy(dataGoingToDisk, data, size);
    // Write the temp copy to the disk
    write_blocks(startingIndex, numBlocks, dataGoingToDisk);
    // Free the temp copy
    free(dataGoingToDisk);
}
void read_data_blocks(void* buffer, size_t size, int diskIndex) {
    int numblocks = bytesToBlockCount(size);
    void* temp = malloc((size_t) numblocks * DISK_BLOCK_SIZE);
    memset(temp, '\0', (size_t) numblocks * DISK_BLOCK_SIZE);
    // Copy the disk block to the temp memory
    read_blocks(diskIndex, numblocks, temp);
    // Copy the desired amount off of temp to output
    memcpy(buffer, temp, size);
    free(temp);
}

/**
 * Save the free bitmap to disk
 */
void load_free_bitmap_from_disk(FreeBitMap* bitMap) {
    read_data_blocks(bitMap, sizeof(FreeBitMap), FREE_BITMAP_BLOCK_INDEX);
}

void load_i_node_cache_from_disk(INodeTable* iNodeTable) {
    read_data_blocks(iNodeTable, sizeof(INodeTable), I_NODE_TABLE_BLOCK_INDEX);
}

void load_directory_cache_from_disk(DirectoryCache* directoryCache) {
    read_data_blocks(directoryCache, sizeof(DirectoryCache), DIRECTORIES_BLOCK_INDEX);
}


void save_local_file_system_to_disk(FreeBitMap* bitmap, INodeTable* iNodeTable, DirectoryCache* directoryCache1) {
    //printf("Saving local filesystem to disk.\n");
    write_data_blocks(bitmap, FREE_BITMAP_BLOCK_INDEX, sizeof(FreeBitMap));
    write_data_blocks(iNodeTable, I_NODE_TABLE_BLOCK_INDEX, sizeof(INodeTable));
    write_data_blocks(directoryCache1, DIRECTORIES_BLOCK_INDEX, sizeof(DirectoryCache));
}

#endif //ASSIGNMENT_3_DISK_ACCESS_H
