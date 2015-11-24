//
// Created by Andrew on 2015-11-22.
//

#ifndef ASSIGNMENT_4_UNFREE_BLOCK_H
#define ASSIGNMENT_4_UNFREE_BLOCK_H

#include <stddef.h>

// The public pointer skips the size int
const size_t PUBLIC_POINTER_OFFSET = sizeof(int);

void* UnFreeBlock_publicPointerToPrivatePointer(void* publicPointer) {
    return publicPointer - PUBLIC_POINTER_OFFSET;
}

size_t UnFreeBlock_getTotalSizeWithMetaData(int size) {
    return size + sizeof(int);
}

void UnFreeBlock_setSize(void* block, int size) {
    *((int*) block) = size;
}

int UnFreeBlock_getSize(void* block) {
    return *((int*) block);
}

void* UnFreeBlock_getPublicPointer(void* block) {
    return block + PUBLIC_POINTER_OFFSET;
}

void UnFreeBlock_construct(void* block, int size) {
    UnFreeBlock_setSize(block, size);
}

#endif //ASSIGNMENT_4_UNFREE_BLOCK_H
