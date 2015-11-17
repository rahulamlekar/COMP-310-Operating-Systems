//
// Created by Andrew on 2015-11-12.
//

#ifndef ASSIGNMENT_3_INDIRECT_BLOCK_POINTER_H
#define ASSIGNMENT_3_INDIRECT_BLOCK_POINTER_H

#include <stdio.h>
#include "../constants.h"

typedef struct indirect_block_pointer {
    int block[DISK_BLOCK_SIZE / sizeof(int)];
} IndirectBlockPointer;

int indirectBlockIndex(int pointerIndex) {
    return pointerIndex - BLOCKS_PER_I_NODE;
}

int isIndexInIndirectBlock(int pointerIndex) {
    return pointerIndex >= BLOCKS_PER_I_NODE;
}

void IndirectBlockPointer_print(IndirectBlockPointer block) {
    printf("IdirectBlockPointer {\n");
    int i;
    for (i = 0; i < (DISK_BLOCK_SIZE / sizeof(int)); i++) {
        printf("[%d = %d\n", i, block.block[i]);
    }
    printf("}\n");
}

#endif //ASSIGNMENT_3_INDIRECT_BLOCK_POINTER_H
