//
// Created by Andrew on 2015-11-22.
//

#ifndef ASSIGNMENT_4_UNFREE_BLOCK_H
#define ASSIGNMENT_4_UNFREE_BLOCK_H

#include <stddef.h>
#include "tag.h"

// The public pointer skips the size int
const size_t PUBLIC_POINTER_OFFSET = sizeof(char) + sizeof(int);

void* UnFreeBlock_publicPointerToPrivatePointer(void* publicPointer) {
    return publicPointer - PUBLIC_POINTER_OFFSET;
}

size_t totalSizeOfUnfreeBlock(size_t innerSize) {
    return PUBLIC_POINTER_OFFSET + innerSize + TAG_SIZE;
}

void UnFreeBlock_setSize(void* block, int size) {
    *((int*) block) = size;
}

int UnFreeBlock_getSize(void* block) {
    return *((int*) block);
}

/**
 * Get the public pointer of an unfree block
 */
void* UnFreeBlock_getPublicPointer(void* block) {
    return block + PUBLIC_POINTER_OFFSET;
}

void UnFreeBlock_construct(void* block, int size) {
    ((char*) block) = TAG_IS_NOT_FREE;
    UnFreeBlock_setSize(block, size);
}

#endif //ASSIGNMENT_4_UNFREE_BLOCK_H
