//
// Created by Andrew on 2015-11-22.
//

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "malloc.h"
#include "utils/constants.h"
#include "structs/free_block.h"
#include "structs/unfree_block.h"

// Allocation policies
#define BEST_FIT_POLICY 1
#define FIRST_FIT_POLICY 0

int allocationPolicy = 0;

// The address at the end of the heap
void* heap_end = NULL;
void* my_brk = NULL;

// Keep track of the first and last free blocks
void* firstFreeBlock = NULL;
void* lastFreeBlock = NULL;

// Interface-defined string
char* my_malloc_error = NULL;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Private functions -- utilities
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int FreeBlockList_getLargestBlockSize(void* head) {
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

static int FreeBlockList_totalInternalBlockSize(void* head) {
    int output = 0;

    void* next = head;
    while (next != NULL) {
        output += FreeBlock_getInternalSize(next);
        // Keep looping
        next = FreeBlock_getNext(next);
    }
    // Return the total size
    return output;
}


/**
 * Get the first available free block that is at least as big as the size param.
 */
static void* FreeBlockList_getFirstAvailable(void* head, int externalSize) {
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
static void* FreeBlockList_getSmallestAvailable(void* head, int externalSize) {
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

static void FreeBlockList_mergeContiguousBlockLeft(void* block) {
    void* left = FreeBlock_getPrev(block);
    if (left != NULL && FreeBlock_getNextContiguousBlock(left) == block) {
        // Set the next pointer of the
        FreeBlock_setNext(left, FreeBlock_getNext(block));
        // Set the size of the newly merged block
        FreeBlock_setInternalSize(left, FreeBlock_getInternalSize(left) + externalSizeOfFreeBlock((size_t) FreeBlock_getInternalSize(block)));

        // We need to handle the case where the last free block pointer gets eliminated
        if (lastFreeBlock == block) {
            lastFreeBlock = left;
        }
    }
}

static void FreeBlockList_mergeContiguousBlockRight(void* block) {
    void* right = FreeBlock_getNext(block);
    if (right != NULL && FreeBlock_getNextContiguousBlock(block) == right) {
        // Set the next pointer of the
        FreeBlock_setNext(block, FreeBlock_getNext(right));
        // Set the size of the newly merged block
        FreeBlock_setInternalSize(block, FreeBlock_getInternalSize(block) + externalSizeOfFreeBlock((size_t) FreeBlock_getInternalSize(right)));

        // We need to handle the case where the last free block pointer gets eliminated
        if (lastFreeBlock == right) {
            lastFreeBlock = block;
        }
    }
}

/**
 * Merge a contiguous bock left or right
 */
static void FreeBlockList_mergeContiguousBlocks(void* block) {
    // Try to merge right
    FreeBlockList_mergeContiguousBlockRight(block);
    // Try to merge left
    FreeBlockList_mergeContiguousBlockLeft(block);
}


/**
 * Remove a free block from the linked list
 */
static void FreeBlockList_remove(void* block) {
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

static void FreeBlockList_insertBeginning(void* newBlock) {
    // This is the new first block
    // Set the new block's prev and next
    FreeBlock_setNext(newBlock, firstFreeBlock);
    FreeBlock_setPrev(newBlock, NULL);
    // Update the old first block
    FreeBlock_setPrev(firstFreeBlock, newBlock);
    // This is the new first free block
    firstFreeBlock = newBlock;
}

static void FreeBlockList_insertEnd(void* newBlock) {
    // This is the new last block
    FreeBlock_setPrev(newBlock, lastFreeBlock);
    FreeBlock_setNext(newBlock, NULL);
    FreeBlock_setNext(lastFreeBlock, newBlock);
    lastFreeBlock = newBlock;
}


/**
 * The linked list is currently empty.  After the insertion, it will only have one member.
 */
static void FreeBlockList_insertOnly(void* newBlock) {
    firstFreeBlock = newBlock;
    lastFreeBlock = newBlock;
    FreeBlock_setNext(newBlock, NULL);
    FreeBlock_setPrev(newBlock, NULL);
}


/**
 * Insert a new block into the linked list at the appropriate location
 */
static void FreeBlockList_insertSomeWhereInList(void* newBlock) {
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

/**
 * Insert a new free block into the appropriate place in the linked list
 */
static void FreeBlockList_insert(void* newBlock) {
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
        // Insert somewhere in the list
        FreeBlockList_insertSomeWhereInList(newBlock);
    }
}


/**
 * Convert a free block to an unfree block
 */
static void FreeBlock_makeUnfree(void* block) {
    // Remove from the free block list
    FreeBlockList_remove(block);
    // Construct an unfree block on top
    size_t newSize = (size_t) FreeBlock_getInternalSize(block) + sizeDiff();
    UnFreeBlock_construct(block, newSize);
}

static void* FreeBlock_resize(void* block, size_t amountToRemove) {
    int freeBlockNewSize = FreeBlock_getInternalSize(block) - externalSizeOfUnfreeBlock(amountToRemove);
    // Free block is smaller, so we just resize this free block
    FreeBlock_setInternalSize(block, freeBlockNewSize);
    // The new unfree block starts at the next part
    void* newUnfreeBlock = FreeBlock_getNextContiguousBlock(block);
    // Construct the unfree block there
    UnFreeBlock_construct(newUnfreeBlock, amountToRemove);
    // Return the unfree block
    return newUnfreeBlock;
}

static void* FreeBlock_split(void* block, int newBlockSize) {
    if (FreeBlock_getInternalSize(block) <= externalSizeOfUnfreeBlock(newBlockSize)) {
        // Make the free block unfree
        FreeBlock_makeUnfree(block);
        return block;
    } else {
        // Resize the existing block
        return FreeBlock_resize(block, (size_t) newBlockSize);
    }
}


/**
 * Grow the heap by n bytes.
 */
static int grow_heap_size(int size) {
    // This should only be called the first time
    if (heap_end == NULL) {
        // Record the beginning of the heap
        heap_end = (void*) sbrk(0);
    }

    // Grow it by the size*2 plus 64 so that if it's a small number then you get 64 extra, and if it's a large number
    // then the double is significant.
    int freeBlockSize = size * 2 + 64;
    // Add size + 64 bytes to the heap
    void* new_brk = (void*) sbrk(externalSizeOfFreeBlock(freeBlockSize));

    if (new_brk == (void*) -1) {
        // There was an error!
        return -1;
    } else {
        my_brk = new_brk;
        // Test: Create a free block in the mem
        FreeBlock_construct(my_brk, freeBlockSize, NULL, NULL);
        FreeBlockList_insert(my_brk);
        return 0;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Public Functions -- The malloc interface
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void *my_malloc(int size) {
    // Make sure that the input is valid
    if (size <= 0) {
        my_malloc_error = "Size cannot be less than 1 byte.";
        return NULL;
    }

    // Always add a little extra data to avoid fragmentation
    size += 2 * sizeof(void*) + 1;

    // Handle growing the heap if necessary
    if (my_brk == NULL || firstFreeBlock == NULL || FreeBlockList_getLargestBlockSize(firstFreeBlock) < size) {
        // Grow the size of the heap appropriately
        if (grow_heap_size(size) == -1) {
            // It didn't work, so set error
            my_malloc_error = "Cannot grow heap.";
            return NULL;
        }
    }

    void* freeBlockThatWillBecomePopulated;
    if (allocationPolicy == BEST_FIT_POLICY) {
        // Allocate best first
        freeBlockThatWillBecomePopulated = FreeBlockList_getSmallestAvailable(firstFreeBlock, externalSizeOfUnfreeBlock(size));
    } else {
        // Allocate first available
        freeBlockThatWillBecomePopulated = FreeBlockList_getFirstAvailable(firstFreeBlock, externalSizeOfUnfreeBlock(size));
    }

    // Allocate the relevant portion of the space
    void* newUnfreeBlock = FreeBlock_split(freeBlockThatWillBecomePopulated, size);
    // Return the public address of the new space, etc.
    return UnFreeBlock_getPublicPointer(newUnfreeBlock);
}

void my_free(void *ptr) {
    // Don't do anything if pointer is null or invalid
    if (ptr == NULL) {
        my_malloc_error = "Cannot free null";
        return;
    }
    // Get the private pointer of the unfree block
    void*block = UnFreeBlock_publicPointerToPrivatePointer(ptr);
    // Convert the unfree block into ao free block
    FreeBlock_construct(block, UnFreeBlock_getInternalSize(block) - sizeDiff(), NULL, NULL);
    // Insert the free block into the linked list
    FreeBlockList_insert(block);
    // Merge with contiguous free blocks
    FreeBlockList_mergeContiguousBlocks(block);

    // Lower brk if last free block is more than 128kb.
    if (lastFreeBlock != NULL) {
        int lastBlockSize = FreeBlock_getExternalSize(lastFreeBlock);
        if (lastBlockSize > 128000 && (lastFreeBlock == my_brk)) {
            void* address = lastFreeBlock;
            // Decrease the size of the memory
            sbrk(0 - FreeBlock_getExternalSize(lastFreeBlock));
            // Record the new brk location
            my_brk = (void*) sbrk(0);
            // Remove it from the liast
            FreeBlockList_remove(lastFreeBlock);
        }
    }

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
    int totalFreeSpace = FreeBlockList_totalInternalBlockSize(firstFreeBlock);
    int totalHeapSize = ((int) sbrk(0)) - ((int) heap_end);
    int totalAllocatedSpace =  totalHeapSize - totalFreeSpace;

    printf("Memory Allocation Information: {\n");
    printf("Total heap size: %d bytes\n", totalHeapSize);
    printf("Allocated bytes in heap: %d\n", totalAllocatedSpace);
    printf("Free bytes in heap: %d\n", totalFreeSpace);
    printf("Largest Contiguous Free Space: %d bytes\n", FreeBlockList_getLargestBlockSize(firstFreeBlock));
    printf("}\n");
}
