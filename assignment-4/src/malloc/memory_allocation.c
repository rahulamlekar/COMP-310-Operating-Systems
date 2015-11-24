//
// Created by Andrew on 2015-11-22.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "memory_allocation.h"
#include "structs/memory.h"
#include "utils/constants.h"
#include "structs/free_block.h"
#include "structs/free_block_list.h"

int allocationPolicy = 0;

// The address of the origin of the heap
void* heapOrigin = NULL;
// The address at the end of the heap
void* my_brk = NULL;
int heapSize = 0;

// Keep track of the first and last free blocks
void* firstFreeBlock = NULL;
void* lastFreeBlock = NULL;

/*
 * Utility Functions
 */


/**
 * Remove a free block from the linked list
 */
void FreeBlockList_remove(void* block) {
    if (firstFreeBlock == lastFreeBlock) {
        // There is only 1 block
        firstFreeBlock = NULL;
        lastFreeBlock = NULL;
    } else if (block == firstFreeBlock) {
        // The block is the first block
        firstFreeBlock = FreeBlock_getNext(block);
        FreeBlock_setPrev(firstFreeBlock, NULL);
    } else if (block == lastFreeBlock) {
        // The block is the last block
        lastFreeBlock = FreeBlock_getPrev(lastFreeBlock);
        FreeBlock_setNext(lastFreeBlock, NULL);
    } else {
        // Remove somewhere from the list
        void* prev = FreeBlock_getPrev(block);
        void* next = FreeBlock_getNext(block);
        // The previous points to the next
        FreeBlock_setNext(prev, next);
        // The next points to the previous
        FreeBlock_setPrev(next, prev);
    }
}

/**
 * Insert a new free block into the appropriate place in the linked list
 */
void FreeBlockList_insert(void* newBlock) {
    if (firstFreeBlock == NULL && lastFreeBlock == NULL) {
        // This will be the only free block
        firstFreeBlock = newBlock;
        lastFreeBlock = newBlock;
        FreeBlock_setNext(newBlock, NULL);
        FreeBlock_setPrev(newBlock, NULL);
    } else if (newBlock < firstFreeBlock) {
        // This is the new first block
        // Set the new block's prev and next
        FreeBlock_setNext(newBlock, firstFreeBlock);
        FreeBlock_setPrev(newBlock, NULL);
        // Update the old first block
        FreeBlock_setPrev(firstFreeBlock, newBlock);
        // This is the new first free block
        firstFreeBlock = newBlock;
    } else if (newBlock > lastFreeBlock) {
        // This is the new last block
        FreeBlock_setPrev(newBlock, lastFreeBlock);
        FreeBlock_setNext(newBlock, NULL);
        FreeBlock_setNext(lastFreeBlock, newBlock);
        lastFreeBlock = NULL;
    } else {
        // Insert somewhere in the list
        void* current = firstFreeBlock;
        void* next;
        while (current != NULL) {
            next = FreeBlock_getNext(current);
            if (current < newBlock && next > newBlock) {
                // The new block goes between current and next
                FreeBlock_setPrev(newBlock, current);
                FreeBlock_setNext(newBlock, next);
                // Set the surrounding blocks
                FreeBlock_setNext(current, newBlock);
                FreeBlock_setPrev(next, newBlock);
                return;
            }
            // Continue iterating
            current = FreeBlock_getNext(current);
        }
    }
}


/*
 * Public Functions
 */

void *my_malloc(int size) {
    if (heapOrigin == NULL) {
        // Get the heap origin
        heapOrigin = sbrk(0);
        // Add size + 64 bytes to the heap
        my_brk = sbrk(size + 64);
        heapSize += size + 64;

        // Test: Create a free block in the mem
        FreeBlock_construct(my_brk, size, NULL, NULL);
        firstFreeBlock = my_brk;
        lastFreeBlock = my_brk;
    }

    // Find out if there's an appropriately sized free block
    if (FreeBlockList_getLargestBlockSize(heapOrigin) < size) {
        // We have to allocate even more space to the
        my_brk = sbrk(size + 64);
        FreeBlock_construct(my_brk, size, NULL, NULL);

        // Add this to the end of the linked list
        FreeBlockList_insert(my_brk);
    }

    printf("Test: %d\n", FreeBlockList_getLargestBlockSize(my_brk));

    void* freeBlockThatWillBecomePopulated;
    if (allocationPolicy == 1) {
        // Allocate best first
        freeBlockThatWillBecomePopulated = FreeBlockList_getSmallestAvailable(firstFreeBlock, size);
    } else {
        // Allocate first available
        freeBlockThatWillBecomePopulated = FreeBlockList_getFirstAvailable(firstFreeBlock, size);
    }

    printf("FirstFreeBlock: %p, lastFreeBlock: %p\n", firstFreeBlock, lastFreeBlock);

    // Handle if it's the first in the list
    FreeBlockList_remove(freeBlockThatWillBecomePopulated);

    // Allocate the relevant portion of the space
    // TODO

    // Return the public address of the new space, etc.
    return NULL;
}

void my_free(void *ptr) {
    // ADd the memory from the pointer to the free linked list
    //int size = *(ptr - INT_SIZE);
}


/**
 * This function specifies the memory allocation policy.
 */
void my_mallopt(int policy) {
    if (policy == 1) {
        // Best-fit
        allocationPolicy = 1;
    } else {
        // First fit
        allocationPolicy = 0;
    }
}

void my_mallinfo() {
    int totalBytes = 0;
    int totalFreeSpace = 0;
    int largestContiguousFreeSpace = 0;

    printf("Memory Allocation Information: {\n");
    printf("Total Bytes Allocated: %d\n", totalBytes);
    printf("Total Free Space: %d\n", totalFreeSpace);
    printf("Largest Contiguous Free Space: %d\n", largestContiguousFreeSpace);
    printf("}\n");
}
