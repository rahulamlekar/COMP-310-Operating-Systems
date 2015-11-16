//
// Created by Andrew on 2015-11-12.
//

#ifndef ASSIGNMENT_3_INDIRECT_BLOCK_POINTER_H
#define ASSIGNMENT_3_INDIRECT_BLOCK_POINTER_H

#include "../constants.h"

typedef struct indirect_block_pointer {
    int block[DISK_BLOCK_SIZE / sizeof(int)];
} IndirectBlockPointer;

int indirectBlockIndex(int pointerIndex) {
    return pointerIndex - BLOCKS_PER_I_NODE;
}


#endif //ASSIGNMENT_3_INDIRECT_BLOCK_POINTER_H