//
// Created by Andrew on 2015-11-22.
//

#ifndef ASSIGNMENT_4_UNFREE_BLOCK_H
#define ASSIGNMENT_4_UNFREE_BLOCK_H

#include <stddef.h>

// The public pointer skips the size int
const size_t PUBLIC_POINTER_OFFSET = sizeof(int);

size_t externalSizeOfUnfreeBlock(size_t internalSize) {
    return PUBLIC_POINTER_OFFSET + internalSize;
}

/**
 * The size difference between a free block and an unfree block
 */
size_t sizeDiff() {
    return totalSizeOfFreeBlock(0) - totalSizeOfUnfreeBlock(0);
}

void* UnFreeBlock_publicPointerToPrivatePointer(void* publicPointer) {
    return publicPointer - PUBLIC_POINTER_OFFSET;
}

void UnFreeBlock_setInternalSize(void *block, int size) {
    *((int*) block) = size;
}

size_t UnFreeBlock_getInternalSize(void *block) {
    return *((int*) block);
}

size_t UnfreeBlock_getExternalSize(void* block) {
    return totalSizeOfUnfreeBlock(UnFreeBlock_getInternalSize(block));
}

/**
 * Get the public pointer of an unfree block
 */
void* UnFreeBlock_getPublicPointer(void* block) {
    return block + PUBLIC_POINTER_OFFSET;
}

void UnFreeBlock_construct(void* block, int size) {
    UnFreeBlock_setSize(block, size);
}

#endif //ASSIGNMENT_4_UNFREE_BLOCK_H
