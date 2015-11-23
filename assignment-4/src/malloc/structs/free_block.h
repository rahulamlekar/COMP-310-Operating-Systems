//
// Created by Andrew on 2015-11-22.
//

#ifndef ASSIGNMENT_4_FREE_BLOCK_H
#define ASSIGNMENT_4_FREE_BLOCK_H

#include <stddef.h>

// Size value comes before prevPointer
const size_t prevPointerOffset = sizeof(int);
// Size value and prevPointer come before nextPointer
const size_t nextPointerOffset = sizeof(int) + sizeof(void*);
const size_t emptyDataOffset = sizeof(int) + 2*sizeof(void*);

/**
 * Get the total size of a free block including the metadata
 */
size_t FreeBlock_getTotalSizeWithMetaData(void* freeBlock) {
    return FreeBlock_getSize(freeBlock) + emptyDataOffset;
}

void FreeBlock_setSize(void* freeBlock, int size) {
    *((int*) freeBlock) = size;
}

int FreeBlock_getSize(void* freeBlock) {
    return *((int*) freeBlock);
}

void FreeBlock_setNext(void* freeBlock, void* value) {
    // It's the third int in the block
    int** nextPointer = freeBlock + nextPointerOffset;
    // Set the value
    *nextPointer = value;
}

void* FreeBlock_getNext(void* freeBlock) {
    int** nextPointer = freeBlock + nextPointerOffset;
    return *nextPointer;
}

void FreeBlock_setPrev(void* freeBlock, void* value) {
    // It's the third int in the block
    int** prevPointer = freeBlock + prevPointerOffset;
    // Set the value
    *prevPointer = value;
}

void* FreeBlock_getPrev(void* freeBlock) {
    int** prevPointer = freeBlock + prevPointerOffset;
    return *prevPointer;
}

/**
 * Get the next contiguous block of memory
 */
void* FreeBlock_getNextContiguousBlock(void* block) {
    return block + FreeBlock_getTotalSizeWithMetaData(block);
}

/**
 * Construct a freeBlock in the heap.
 */
void FreeBlock_construct(void* pointer, int size, void* prev, void* next) {
    // Set the size
    FreeBlock_setSize(pointer, size);
    // Set the previous and next values
    FreeBlock_setPrev(pointer, prev);
    FreeBlock_setNext(pointer, next);
}


#endif //ASSIGNMENT_4_FREE_BLOCK_H
