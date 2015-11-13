//
// Created by Andrew Fogarty on 2015-11-08.
//

#ifndef ASSIGNMENT_3_DISK_ACCESS_H
#define ASSIGNMENT_3_DISK_ACCESS_H

#include <stdlib.h>
#include "../data_structures/i_node.h"
#include "../constants.h"
#include "../disk_emu.h"
#include "../data_structures/free_bitmap.h"
#include "../data_structures/i_node_table.h"

/**
 * Load the nTh iNode off of disk
 */
//INode* load_i_node_from_disk(int iNodeIndex) {
//    INode* output = malloc(sizeof(INode));
//    read_blocks(I_NODE_TABLE_BLOCK_INDEX + iNodeIndex, 1, output);
//    return output;
//}

/**
 * Fill a disk block with empty space.
 */
void erase_disk_block(int index) {
    void* empty = malloc(DISK_BLOCK_SIZE);
    write_blocks(index, 1, empty);
    free(empty);
}

/**
 * Save an iNode to the disk
 */
//void save_i_node_to_disk(int iNodeIndex, INode* node) {
//    // Erase previously existing data for safety
//    erase_disk_block(I_NODE_TABLE_BLOCK_INDEX + iNodeIndex);
//    // Write the iNode
//    write_blocks(I_NODE_TABLE_BLOCK_INDEX + iNodeIndex, 1, node);
//}
/**
 * Erase an iNode from disk
 */
//void delete_i_node_from_disk(int iNodeIndex) {
//    erase_disk_block(I_NODE_TABLE_BLOCK_INDEX + iNodeIndex);
//}

void read_data_block(int index, void* buffer) {
    read_blocks(DATA_BLOCK_TABLE_INDEX + index, 1, buffer);
}
void write_data_block(int index, void* buffer) {
    write_blocks(DATA_BLOCK_TABLE_INDEX + index, 1, buffer);
}

/**
 * Write arbitrarily large data to the disk from a particulal index
 */
void write_data_blocks(void* data, int startingIndex, size_t size) {
    int numBlocks = bytesToBlockCount(size);
    printf("Writing %d bytes to %d blocks.\n", size, numBlocks);
    // Make a blank copy of the correct number of blocks
    void* dataGoingToDisk = malloc(numBlocks * DISK_BLOCK_SIZE);
    // Copy the incoming data to the temp copy
    memcpy(dataGoingToDisk, data, size);
    // Write the temp copy to the disk
    write_blocks(startingIndex, numBlocks, dataGoingToDisk);
    // Free the temp copy
    free(dataGoingToDisk);
}
void read_data_blocks(void* buffer, size_t size, int diskIndex) {
    int numblocks = bytesToBlockCount(size);
    void* temp = malloc(numblocks * DISK_BLOCK_SIZE);
    // Copy the disk block to the temp memory
    read_blocks(diskIndex, numblocks, temp);
    // Copy the desired amount off of temp to output
    memcpy(buffer, temp, size);
    free(temp);
}

/**
 * Save the free bitmap to disk
 */
FreeBitMap* load_free_bitmap_from_disk() {
    FreeBitMap* output = malloc(sizeof(FreeBitMap));
    read_data_blocks(output, sizeof(FreeBitMap), FREE_BITMAP_BLOCK_INDEX);
    return output;
}

INodeTable* load_i_node_cache_from_disk() {
    INodeTable* output = malloc(sizeof(INodeTable));
    read_data_blocks(output, sizeof(INodeTable), I_NODE_TABLE_BLOCK_INDEX);
    return output;
}

DirectoryCache* load_directory_cache_from_disk() {
    DirectoryCache* output = malloc(sizeof(DirectoryCache));
    read_data_blocks(output, sizeof(DirectoryCache), I_NODE_TABLE_BLOCK_INDEX);
    return output;
}


void save_local_file_system_to_disk(FreeBitMap* bitmap, INodeTable* iNodeTable, DirectoryCache* directoryCache1) {
    printf("Saving local filesystem to disk.\n");
    // Save the free bitmap
    write_data_blocks(bitmap, FREE_BITMAP_BLOCK_INDEX, sizeof(FreeBitMap));
    printf("Saved free bitmap.");
    // Save the iNodeTable
    write_data_blocks(iNodeTable, I_NODE_TABLE_BLOCK_INDEX, sizeof(INodeTable));
    printf("Saved iNode table.");
//    // Save the directory cache
    write_data_blocks(directoryCache1, DIRECTORIES_BLOCK_INDEX, sizeof(DirectoryCache));
    printf("Saved directory cache.");
}

#endif //ASSIGNMENT_3_DISK_ACCESS_H
