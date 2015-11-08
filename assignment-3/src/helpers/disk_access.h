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

/**
 * Load the nTh iNode off of disk
 */
INode* load_i_node_from_disk(int iNodeIndex) {
    INode* output = malloc(sizeof(INode));
    read_blocks(I_NODE_TABLE_BLOCK_INDEX + iNodeIndex, 1, output);
    return output;
}

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
void save_i_node_to_disk(int iNodeIndex, INode* node) {
    // Erase previously existing data for safety
    erase_disk_block(I_NODE_TABLE_BLOCK_INDEX + iNodeIndex);
    // Write the iNode
    write_blocks(I_NODE_TABLE_BLOCK_INDEX + iNodeIndex, 1, node);
}
/**
 * Erase an iNode from disk
 */
void delete_i_node_from_disk(int iNodeIndex) {
    erase_disk_block(I_NODE_TABLE_BLOCK_INDEX + iNodeIndex);
}

/**
 * Save the free bitmap to disk
 */
void save_free_bitmap_to_disk(FreeBitMap* bitMap) {
    // Erase previously existing data for safety
    erase_disk_block(FREE_BITMAP_BLOCK_INDEX);
    // Write the bitmap to disk
    write_blocks(FREE_BITMAP_BLOCK_INDEX, 1, bitMap);
}
FreeBitMap* load_free_bitmap_from_disk() {
    FreeBitMap* output = malloc(sizeof(FreeBitMap));

    read_blocks(FREE_BITMAP_BLOCK_INDEX, 1, output);
    return output;
}

#endif //ASSIGNMENT_3_DISK_ACCESS_H
