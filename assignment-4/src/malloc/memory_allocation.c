//
// Created by Andrew on 2015-11-22.
//

#include <stdlib.h>
#include <stdio.h>
#include "memory_allocation.h"
#include "utils/constants.h"
#include "structs/free_block.h"
#include "structs/free_block_list.h"
#include "structs/unfree_block.h"

// Allocation policies
#define BEST_FIT_POLICY 1
#define FIRST_FIT_POLICY 0

int allocationPolicy = 0;

// The address at the end of the heap
void* my_brk = NULL;
int heapSize = 0;

// Keep track of the first and last free blocks
void* firstFreeBlock = NULL;
void* lastFreeBlock = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Private functions -- utilities
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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

void FreeBlockList_insertBeginning(void* newBlock) {
    // This is the new first block
    // Set the new block's prev and next
    FreeBlock_setNext(newBlock, firstFreeBlock);
    FreeBlock_setPrev(newBlock, NULL);
    // Update the old first block
    FreeBlock_setPrev(firstFreeBlock, newBlock);
    // This is the new first free block
    firstFreeBlock = newBlock;
}

void FreeBlockList_insertEnd(void* newBlock) {
    // This is the new last block
    FreeBlock_setPrev(newBlock, lastFreeBlock);
    FreeBlock_setNext(newBlock, NULL);
    FreeBlock_setNext(lastFreeBlock, newBlock);
    lastFreeBlock = newBlock;
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
        // New first block
        FreeBlockList_insertBeginning(newBlock);
    } else if (newBlock > lastFreeBlock) {
        // New last block
        FreeBlockList_insertEnd(newBlock);
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


/**
 * Convert a free block to an unfree block
 */
void FreeBlock_makeUnfree(void* block) {
    // Remove from the free block list
    FreeBlockList_remove(block);
    // Construct an unfree block on top
    printf("OldSize: %d\n", FreeBlock_getInternalSize(block));
    size_t newSize = (size_t) FreeBlock_getInternalSize(block) + sizeDiff();
    printf("NewSize: %d\n", newSize);
    UnFreeBlock_construct(block, newSize);
}

void* FreeBlock_resize(void* block, size_t amountToRemove) {
    printf("Test: %d, %d\n", FreeBlock_getInternalSize(block), amountToRemove);
    printf("Test2: %d, %d\n", FreeBlock_getInternalSize(block), externalSizeOfUnfreeBlock(amountToRemove));
    int freeBlockNewSize = FreeBlock_getInternalSize(block) - externalSizeOfUnfreeBlock(amountToRemove);

    FreeBlock_print(block);

    // Free block is smaller, so we just resize this free block
    FreeBlock_setInternalSize(block, freeBlockNewSize);
    printf("Resized free block to %d bytes\n", freeBlockNewSize);

    FreeBlock_print(block);

    // The new unfree block starts at the next part
    void* newUnfreeBlock = FreeBlock_getNextContiguousBlock(block);
    printf("newUnfreeBlock: %p\n", newUnfreeBlock);
    // Construct the unfree block there
    UnFreeBlock_construct(newUnfreeBlock, amountToRemove);

    FreeBlock_print(block);

    // Return the unfree block
    return newUnfreeBlock;
}

void* FreeBlock_split(void* block, int newBlockSize) {
    int oldFreeBlockSize = FreeBlock_getInternalSize(block);
    printf("Old free Block internal size: %d, newBlockSize: %d\n", oldFreeBlockSize, newBlockSize);
    if (oldFreeBlockSize <= externalSizeOfUnfreeBlock(newBlockSize)) {
        printf("Equal case\n");
        // Make the free block unfree
        FreeBlock_makeUnfree(block);
        return block;
    } else {
        printf("not equal case\n");
        // Resize the existing block
        return FreeBlock_resize(block, (size_t) newBlockSize);
    }
}

void grow_heap_size(int size) {
    // Grow it by the size*2 plus 64 so that if it's a small number then you get 64 extra, and if it's a large number
    // then the double is significant.
    int freeBlockSize = size * 2 + 64;
    // Add size + 64 bytes to the heap
    my_brk = sbrk(externalSizeOfFreeBlock(freeBlockSize));
    // Test: Create a free block in the mem
    FreeBlock_construct(my_brk, freeBlockSize, NULL, NULL);
    FreeBlockList_insert(my_brk);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Public Functions -- The malloc interface
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void *my_malloc(int size) {
    printf("malloc(%d)\n", size);

    // Always add a little extra data to avoid fragmentation
    size += sizeof(int) + 1;

    printf("Largest free size: %d\n", FreeBlockList_getLargestBlockSize(firstFreeBlock));
    if (my_brk == NULL || FreeBlockList_getLargestBlockSize(firstFreeBlock) < size) {
        // Grow the size of the heap appropriately
        grow_heap_size(size);
    }

    FreeBlockList_print(firstFreeBlock);

    printf("Test: %d\n", FreeBlockList_getLargestBlockSize(my_brk));

    void* freeBlockThatWillBecomePopulated;
    if (allocationPolicy == BEST_FIT_POLICY) {
        // Allocate best first
        freeBlockThatWillBecomePopulated = FreeBlockList_getSmallestAvailable(firstFreeBlock, externalSizeOfUnfreeBlock(size));
    } else {
        // Allocate first available
        freeBlockThatWillBecomePopulated = FreeBlockList_getFirstAvailable(firstFreeBlock, externalSizeOfUnfreeBlock(size));
    }

    printf("FirstFreeBlock: %p, lastFreeBlock: %p\n", firstFreeBlock, lastFreeBlock);

    printf("Internal Size of freeBlockThatWillBecomePopulated: %d\n", FreeBlock_getInternalSize(freeBlockThatWillBecomePopulated));

    FreeBlockList_print(firstFreeBlock);

    // Allocate the relevant portion of the space
    void* newUnfreeBlock = FreeBlock_split(freeBlockThatWillBecomePopulated, size);

    printf("Split successful!\n");

    FreeBlockList_print(firstFreeBlock);

    printf("my_malloc() about to return\n");

    // Return the public address of the new space, etc.
    return UnFreeBlock_getPublicPointer(newUnfreeBlock);
}

void my_free(void *ptr) {
    printf("my_free(%p)\n", ptr);
    FreeBlockList_print(firstFreeBlock);
    // Get the private pointer of the unfree block
    void*block = UnFreeBlock_publicPointerToPrivatePointer(ptr);
    printf("got unfree block\n");

    printf("uf block external size: %d\n", UnfreeBlock_getExternalSize(block));
    printf("uf block internal size: %d\n", UnFreeBlock_getInternalSize(block));

    // Convert the unfree block into ao free block
    FreeBlock_construct(block, UnFreeBlock_getInternalSize(block) - sizeDiff(), NULL, NULL);

    FreeBlock_print(block);

    printf("block external size: %d\n", FreeBlock_getExternalSize(block));

    printf("unfreeBlock: %d, firstBlock: %d, diff: %d\n", block, firstFreeBlock, block - firstFreeBlock);



    FreeBlockList_print(firstFreeBlock);

    printf("About to insert the unfree block");
    // Insert the free block into the linked list
    FreeBlockList_insert(block);

    printf("Finished inserting\n");
    FreeBlock_print(block);


    FreeBlockList_print(firstFreeBlock);
    // Merge with contiguous free blocks
    FreeBlockList_mergeContiguousBlocks(block);
    printf("end my_free()\n");
}


/**
 * This function specifies the memory allocation policy.
 */
void my_mallopt(int policy) {
    if (policy == 1) {
        // Best-fit
        allocationPolicy = BEST_FIT_POLICY;
    } else {
        // First fit
        allocationPolicy = FIRST_FIT_POLICY;
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
