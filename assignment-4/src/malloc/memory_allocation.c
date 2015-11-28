//
// Created by Andrew on 2015-11-22.
//

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "memory_allocation.h"
#include "utils/constants.h"
#include "structs/free_block.h"
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

int FreeBlockList_getLargestBlockSize(void* head) {
    int largest = 0;
    int currentSize = 0;

    void* next = head;
    while (next != NULL) {
        currentSize = FreeBlock_getInternalSize(next);

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
void* FreeBlockList_getFirstAvailable(void* head, int externalSize) {
    void* next = head;
    while (next != NULL) {
        // Basically, we loop through the free block linkedlist until we find something big enough
        if (FreeBlock_getExternalSize(next) >= externalSize) {
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
void* FreeBlockList_getSmallestAvailable(void* head, int externalSize) {
    void* smallestBlock = NULL;
    int smallestSize = INT_MAX;

    int currentSize = 0;
    void* next = head;
    while (next != NULL) {
        currentSize = FreeBlock_getExternalSize(next);
        if (currentSize >= externalSize && currentSize < smallestSize) {
            smallestSize = currentSize;
            smallestBlock = next;
        }
        // Keep looping
        next = FreeBlock_getNext(next);
    }

    // Will return an actual block if success, null if failure
    return smallestBlock;
}

void FreeBlockList_mergeContiguousBlockLeft(void* block) {
    void* left = FreeBlock_getPrev(block);
    if (left != NULL && FreeBlock_getNextContiguousBlock(left) == block) {
        printf("Merging FreeBlock left!");
        // Set the next pointer of the
        FreeBlock_setNext(left, FreeBlock_getNext(block));
        // Set the size of the newly merged block
        FreeBlock_setInternalSize(left, FreeBlock_getInternalSize(left) + externalSizeOfFreeBlock((size_t) FreeBlock_getInternalSize(block)));

        // We need to handle the case where the last free block pointer gets eliminated
        if (lastFreeBlock == block) {
            printf("LastFreeBlock moving left!\n");
            lastFreeBlock = left;
        }
    }
}

void FreeBlockList_mergeContiguousBlockRight(void* block) {
    void* right = FreeBlock_getNext(block);
    if (right != NULL && FreeBlock_getNextContiguousBlock(block) == right) {
        printf("Merging FreeBlock right!\n");
        // Set the next pointer of the
        FreeBlock_setNext(block, FreeBlock_getNext(right));
        // Set the size of the newly merged block
        FreeBlock_setInternalSize(block, FreeBlock_getInternalSize(block) + externalSizeOfFreeBlock((size_t) FreeBlock_getInternalSize(right)));

        // We need to handle the case where the last free block pointer gets eliminated
        if (lastFreeBlock == right) {
            printf("LastFreeBlock moving left from right!\n");
            lastFreeBlock = block;
        }
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
    printf("Free Block List: {\n");
    void* next = head;
    while (next != NULL) {
        printf("Block %p, size: %d, prev: %p, next: %p\n", next, FreeBlock_getInternalSize(next), FreeBlock_getPrev(next), FreeBlock_getNext(next));
        next = FreeBlock_getNext(next);
    }
    printf("}\n");
}


/**
 * Remove a free block from the linked list
 */
void FreeBlockList_remove(void* block) {
    printf("firstFreeBlock: %p, lastFreeBlock: %p\n", firstFreeBlock, lastFreeBlock);
    if (firstFreeBlock == lastFreeBlock) {
        printf("Removing only free block\n");
        // There is only 1 block
        firstFreeBlock = NULL;
        lastFreeBlock = NULL;
    } else if (block == firstFreeBlock) {
        printf("Removing first free block\n");
        // The block is the first block
        firstFreeBlock = FreeBlock_getNext(block);
        FreeBlock_setPrev(firstFreeBlock, NULL);
    } else if (block == lastFreeBlock) {
        printf("Removing last free block\n");
        // The block is the last block
        lastFreeBlock = FreeBlock_getPrev(lastFreeBlock);
        FreeBlock_setNext(lastFreeBlock, NULL);
    } else {
        printf("Removing somewhere in list\n");
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
    printf("Inserting free block at end of list\n");
    // This is the new last block
    FreeBlock_setPrev(newBlock, lastFreeBlock);
    FreeBlock_setNext(newBlock, NULL);
    FreeBlock_setNext(lastFreeBlock, newBlock);
    lastFreeBlock = newBlock;
}


/**
 * The linked list is currently empty.  After the insertion, it will only have one member.
 */
void FreeBlockList_insertOnly(void* newBlock) {
    firstFreeBlock = newBlock;
    lastFreeBlock = newBlock;
    FreeBlock_setNext(newBlock, NULL);
    FreeBlock_setPrev(newBlock, NULL);
}


/**
 * Insert a new block into the linked list at the appropriate location
 */
void FreeBlockList_insertSomeWhereInList(void* newBlock) {
    printf("Inserting the freeblock somewhere in the list\n");
    void* current = firstFreeBlock;
    void* next;
    printf("Current: %p\n", current);
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

/**
 * Insert a new free block into the appropriate place in the linked list
 */
void FreeBlockList_insert(void* newBlock) {
    if (firstFreeBlock == NULL && lastFreeBlock == NULL) {
        // This will be the only free block
        FreeBlockList_insertOnly(newBlock);
    } else if (newBlock < firstFreeBlock) {
        // New first block
        FreeBlockList_insertBeginning(newBlock);
    } else if (newBlock > lastFreeBlock) {
        // New last block
        FreeBlockList_insertEnd(newBlock);
    } else {
        printf("Insert somewhere in list\n");
        // Insert somewhere in the list
        FreeBlockList_insertSomeWhereInList(newBlock);
    }
}


/**
 * Convert a free block to an unfree block
 */
void FreeBlock_makeUnfree(void* block) {
    printf("FreeBlock_makeUnfree(%p)\n", block);
    FreeBlockList_print(firstFreeBlock);
    // Remove from the free block list
    FreeBlockList_remove(block);
    printf("Removed block\n");
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
    printf("Old free Block internal size: %d, newBlockSize: %d\n", FreeBlock_getInternalSize(block), newBlockSize);
    if (FreeBlock_getInternalSize(block) <= externalSizeOfUnfreeBlock(newBlockSize)) {
        printf("is equal case\n");
        // Make the free block unfree
        FreeBlock_makeUnfree(block);
        printf("made block unfree\n");
        return block;
    } else {
        printf("not equal case\n");
        // Resize the existing block
        return FreeBlock_resize(block, (size_t) newBlockSize);
    }
}

void grow_heap_size(int size) {
    printf("Growing the heap!\n");
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
    printf("my_malloc(%d)\n", size);

    // Always add a little extra data to avoid fragmentation
    size += 2 * sizeof(void*) + 1;

    printf("Largest free size: %d\n", FreeBlockList_getLargestBlockSize(firstFreeBlock));
    if (my_brk == NULL || FreeBlockList_getLargestBlockSize(firstFreeBlock) < size) {
        // Grow the size of the heap appropriately
        grow_heap_size(size);
    }

    FreeBlockList_print(firstFreeBlock);

    printf("testlol\n");


    void* freeBlockThatWillBecomePopulated;
    if (allocationPolicy == BEST_FIT_POLICY) {
        printf("Executing best fit policy\n");
        // Allocate best first
        freeBlockThatWillBecomePopulated = FreeBlockList_getSmallestAvailable(firstFreeBlock, externalSizeOfUnfreeBlock(size));
    } else {
        printf("Executing first fit policy\n");
        // Allocate first available
        freeBlockThatWillBecomePopulated = FreeBlockList_getFirstAvailable(firstFreeBlock, externalSizeOfUnfreeBlock(size));
    }

    printf("FirstFreeBlock: %p, lastFreeBlock: %p\n", firstFreeBlock, lastFreeBlock);

    printf("Internal Size of freeBlockThatWillBecomePopulated: %d\n", FreeBlock_getInternalSize(freeBlockThatWillBecomePopulated));

    FreeBlockList_print(firstFreeBlock);

    // Allocate the relevant portion of the space
    void* newUnfreeBlock = FreeBlock_split(freeBlockThatWillBecomePopulated, size);

    printf("Split successful!\n");

    printf("FirstFreeBlock: %p, lastFreeBlock: %p\n", firstFreeBlock, lastFreeBlock);

    FreeBlockList_print(firstFreeBlock);

    printf("my_malloc() about to return\n");

    // Return the public address of the new space, etc.
    return UnFreeBlock_getPublicPointer(newUnfreeBlock);
}

void my_free(void *ptr) {
    printf("\n\nmy_free(%p)\n", ptr);
    printf("FirstFreeBlock: %p, lastFreeBlock: %p\n", firstFreeBlock, lastFreeBlock);
    FreeBlockList_print(firstFreeBlock);
    // Get the private pointer of the unfree block
    void*block = UnFreeBlock_publicPointerToPrivatePointer(ptr);
    printf("got unfree block\n");

    printf("unfree block external size: %d\n", UnfreeBlock_getExternalSize(block));
    printf("unfree block internal size: %d\n", UnFreeBlock_getInternalSize(block));

    if (UnFreeBlock_getInternalSize(block) - sizeDiff() < 1) {
        // We can't have a negative block, so we have to merge it!

        // TODO
    }

    // Convert the unfree block into ao free block
    FreeBlock_construct(block, UnFreeBlock_getInternalSize(block) - sizeDiff(), NULL, NULL);

    FreeBlock_print(block);

    printf("block external size: %d\n", FreeBlock_getExternalSize(block));

    printf("unfreeBlock: %d, firstBlock: %d, diff: %d\n", block, firstFreeBlock, block - firstFreeBlock);

    printf("About to insert the unfree block\n");
    // Insert the free block into the linked list
    FreeBlockList_insert(block);

    printf("Finished inserting\n");
    FreeBlock_print(block);

    printf("FirstFreeBlock: %p, lastFreeBlock: %p\n", firstFreeBlock, lastFreeBlock);
    FreeBlockList_print(firstFreeBlock);

    printf("Merging the continuous blocks\n");
    // Merge with contiguous free blocks
    FreeBlockList_mergeContiguousBlocks(block);

    printf("FirstFreeBlock: %p, lastFreeBlock: %p\n", firstFreeBlock, lastFreeBlock);
    FreeBlockList_print(firstFreeBlock);

    printf("end my_free()\n\n\n");
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
