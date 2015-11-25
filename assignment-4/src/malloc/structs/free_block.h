//
// Created by Andrew on 2015-11-22.
//

#ifndef ASSIGNMENT_4_FREE_BLOCK_H
#define ASSIGNMENT_4_FREE_BLOCK_H

#include <stddef.h>

// Size value comes before prevPointer
const size_t PREV_POINTER_OFFSET = sizeof(int);
// Size value and prevPointer come before nextPointer
const size_t NEXT_POINTER_OFFSET = sizeof(int) + sizeof(void*);
const size_t EMPTY_DATA_OFFSET = sizeof(int) + 2 * sizeof(void*);


size_t externalSizeOfFreeBlock(size_t innerSize) {
    return EMPTY_DATA_OFFSET + innerSize;
}

void FreeBlock_setInternalSize(void *freeBlock, int size) {
    *((int*) freeBlock) = size;
}

size_t FreeBlock_getInternalSize(void *freeBlock) {
    return *((int*) freeBlock);
}

/**
 * Get the external size of a free block.
 */
size_t FreeBlock_getExternalSize(void* freeBlock) {
    return externalSizeOfFreeBlock(FreeBlock_getInternalSize(freeBlock));
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
 * Get the next contiguous block of memory
 */
void* FreeBlock_getNextContiguousBlock(void* block) {
    return block + FreeBlock_getExternalSize(block);
}

/**
 * Construct a freeBlock in the heap.
 */
void FreeBlock_construct(void* pointer, int size, void* prev, void* next) {
    // Set the size
    FreeBlock_setInternalSize(pointer, size);
    // Set the previous and next values
    FreeBlock_setPrev(pointer, prev);
    FreeBlock_setNext(pointer, next);
}

void FreeBlock_print(void* pointer) {
    printf("FreeBlock {\n");
    printf("    internal size: %d\n", FreeBlock_getInternalSize(pointer));
    printf("    external size: %d\n", FreeBlock_getExternalSize(pointer));
    printf("    prev: %p\n", FreeBlock_getPrev(pointer));
    printf("    next: %p\n", FreeBlock_getNext(pointer));
    printf("}\n");
}


#endif //ASSIGNMENT_4_FREE_BLOCK_H
