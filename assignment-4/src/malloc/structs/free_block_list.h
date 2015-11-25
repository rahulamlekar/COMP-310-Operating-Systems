//
// Created by Andrew on 2015-11-22.
//

#ifndef ASSIGNMENT_4_FREE_BLOCK_LIST_H
#define ASSIGNMENT_4_FREE_BLOCK_LIST_H

#include <stddef.h>
#include <limits.h>
#include "free_block.h"

int FreeBlockList_getLargestBlockSize(void* head) {
    int largest = 0;
    int currentSize = 0;

    void* next = head;
    while (next != NULL) {
        currentSize = FreeBlock_getSize(next);

        // Update the size if necessary
        if (currentSize > largest) {
            largest = currentSize;
        }

        // Keep looping
        next = FreeBlock_getNext(next);
    }
    // Return the largest size
    return largest;
}


/**
 * Get the first available free block that is at least as big as the size param.
 */
void* FreeBlockList_getFirstAvailable(void* head, int size) {
    void* next = head;
    while (next != NULL) {
        // Basically, we loop through the free block linkedlist until we find something big enough
        if (FreeBlock_getSize(next) >= size) {
            return next;
        }
        // It's not big enough, so keep searching
        next = FreeBlock_getNext(next);
    }

    // None available!
    return NULL;
}

/**
 * Get the smallesst available block that is at least as big as size
 */
void* FreeBlockList_getSmallestAvailable(void* head, int size) {
    void* smallestBlock = NULL;
    int smallestSize = INT_MAX;

    int currentSize = 0;
    void* next = head;
    while (next != NULL) {
        currentSize = FreeBlock_getSize(next);
        if (currentSize >= size && currentSize < smallestSize) {
            smallestSize = currentSize;
            smallestBlock = next;
        }
        // Keep looping
        next = FreeBlock_getNext(next);
    }

    // Will return an actual block if success, null if failure
    return smallestBlock;
}

void* FreeBlockList_getFreeBlockOfAddress(void* head, void* address) {
    void* next = head;
    while(next != NULL) {
        // If the address falls within the limits of this block, return it
        if (address >= next && address < FreeBlock_getNextContiguousBlock(next)) {
            return next;
        }

        // Continue looping
        next = FreeBlock_getNext(next);
    }

    // Address is in an unfree block!
    return NULL;
}

void FreeBlockList_mergeContiguousBlockLeft(void* block) {
    void* left = FreeBlock_getPrev(block);
    if (left != NULL && FreeBlock_getNextContiguousBlock(left) == block) {
        printf("Merging FreeBlock left!");
        // Set the next pointer of the
        FreeBlock_setNext(left, FreeBlock_getNext(block));
        // Set the size of the newly merged block
        FreeBlock_setSize(left, FreeBlock_getSize(left) + totalSizeOfFreeBlock((size_t) FreeBlock_getSize(block)));
    }
}

void FreeBlockList_mergeContiguousBlockRight(void* block) {
    void* right = FreeBlock_getNext(block);
    if (right != NULL && FreeBlock_getNextContiguousBlock(block) == right) {
        printf("Merging FreeBlock !");
        // Set the next pointer of the
        FreeBlock_setNext(block, FreeBlock_getNext(right));
        // Set the size of the newly merged block
        FreeBlock_setSize(block, FreeBlock_getSize(block) + totalSizeOfFreeBlock((size_t) FreeBlock_getSize(right)));
    }
}

/**
 * Merge a contiguous bock left or right
 */
void FreeBlockList_mergeContiguousBlocks(void* block) {
    // Try to merge right
    FreeBlockList_mergeContiguousBlockRight(block);
    // Try to merge left
    FreeBlockList_mergeContiguousBlockLeft(block);

}

void FreeBlockList_print(void* head) {
    printf("--\n");
    printf("Free Block List: \n");
    void* next = head;
    while (next != NULL) {
        printf("Block %p, size: %d\n", next, FreeBlock_getSize(next));
        next = FreeBlock_getNext(next);
    }
    printf("--\n");
}

#endif //ASSIGNMENT_4_FREE_BLOCK_LIST_H
