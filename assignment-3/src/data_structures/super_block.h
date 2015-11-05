//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_SUPER_BLOCK_H
#define ASSIGNMENT_3_SUPER_BLOCK_H

typedef struct super_block {
    int magic;
    int block_size;
    int file_system_size;
    int i_node_table_length;
    int root_directory_i_node;
} SuperBlock;

SuperBlock construct_super_block() {
    return (SuperBlock) {
            0xAABB0005,
            512,
            16, // we want 16 blocks
            16, // table of length 16
            0 // Root directory at iNode 0
    };
}

#endif //ASSIGNMENT_3_SUPER_BLOCK_H
