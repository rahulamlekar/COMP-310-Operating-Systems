//
// Created by Andrew on 2015-11-22.
//

#ifndef ASSIGNMENT_4_FREE_BLOCK_H
#define ASSIGNMENT_4_FREE_BLOCK_H

#include <stddef.h>
#include "tag.h"

const size_t SIZE_POINTER_OFFSET = sizeof(char);
// Size value comes before prevPointer
const size_t PREV_POINTER_OFFSET = sizeof(char) + sizeof(int);
// Size value and prevPointer come before nextPointer
const size_t NEXT_POINTER_OFFSET = sizeof(char) + sizeof(int) + sizeof(void*);
const size_t EMPTY_DATA_OFFSET = sizeof(char) + sizeof(int) + 2 * sizeof(void*);


size_t totalSizeOfFreeBlock(size_t innerSize) {
    return EMPTY_DATA_OFFSET + innerSize + TAG_SIZE;
}

void FreeBlock_setSize(void* freeBlock, int size) {
    *((int*) freeBlock) = size;
}

int FreeBlock_getSize(void* freeBlock) {
    return *((int*) freeBlock);
}

void FreeBlock_setNext(void* freeBlock, void* value) {
    // It's the third int in the block
    int** nextPointer = freeBlock + NEXT_POINTER_OFFSET;
    // Set the value
    *nextPointer = value;
}

void* FreeBlock_getNext(void* freeBlock) {
    int** nextPointer = freeBlock + NEXT_POINTER_OFFSET;
    return *nextPointer;
}

void FreeBlock_setPrev(void* freeBlock, void* value) {
    // It's the third int in the block
    int** prevPointer = freeBlock + PREV_POINTER_OFFSET;
    // Set the value
    *prevPointer = value;
}

void* FreeBlock_getPrev(void* freeBlock) {
    int** prevPointer = freeBlock + PREV_POINTER_OFFSET;
    return *prevPointer;
}

/**
 * Get the total size of a free block including the metadata
 */
size_t FreeBlock_getTotalSizeWithMetaData(void* freeBlock) {
    return totalSizeOfFreeBlock((size_t) FreeBlock_getSize(freeBlock));
}

/**
 * Get the next contiguous block of memory
 */
void* FreeBlock_getNextContiguousBlock(void* block) {
    return block + FreeBlock_getTotalSizeWithMetaData(block);
}

/**
 * Find out if the next contiguous block is free
 */
int FreeBlock_nextIsFree(void* block) {
    char tagValue = *((char*) FreeBlock_getNextContiguousBlock(block));
    return tagIsFree(tagValue);
}

/**
 * Construct a freeBlock in the heap.
 */
void FreeBlock_construct(void* pointer, int size, void* prev, void* next) {
    // Set the opening tag
    *((char*) pointer) = TAG_IS_FREE;
    // Set the size
    FreeBlock_setSize(pointer, size);
    // Set the previous and next values
    FreeBlock_setPrev(pointer, prev);
    FreeBlock_setNext(pointer, next);
    // Set the ending tag
    *(((char*) pointer) + EMPTY_DATA_OFFSET + size) = TAG_IS_FREE;
}


#endif //ASSIGNMENT_4_FREE_BLOCK_H
