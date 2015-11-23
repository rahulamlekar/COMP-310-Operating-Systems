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

/**
 * Pointer to the first free block
 */
void* firstFreeBlock = NULL;

void *my_malloc(int size) {
    if (heapOrigin == NULL) {
        // Get the heap origin
        heapOrigin = sbrk(0);
        // Add size + 64 bytes to the heap
        my_brk = sbrk(size + 64);
        heapSize += size + 64;

        // Test: Create a free block in the mem
        FreeBlock_construct(my_brk, size, NULL, NULL);
    }

    // Find out if there's an appropriately sized free block
    if (FreeBlockList_getLargestBlockSize(heapOrigin) < size) {
        // We have to allocate even more space to the
        my_brk = sbrk(size + 64);
        FreeBlock_construct(my_brk, size, NULL, NULL);
    }

    printf("Test: %d\n", FreeBlockList_getLargestBlockSize(my_brk));


    // If no such block, then allocate some new space and use it
    void* newBlock = sbrk(size + 64);
    heapSize += size + 64;


    // Set up the new mem

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
