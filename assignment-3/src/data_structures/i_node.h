//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_I_NODE_H
#define ASSIGNMENT_3_I_NODE_H

typedef struct i_node {
    int mode;
    int link_cnt;
    int uid;
    int gid;
    int size;
    int pointer[BLOCKS_PER_I_NODE];
    int ind_pointer;
} INode;

void INode_copy(INode* dest, INode* source) {
    // Copy all the data
    dest->size = source->size;
    dest->gid = source->gid;
    dest->uid = source->uid;
    dest->ind_pointer = source->ind_pointer;
    dest->link_cnt = source->link_cnt;
    dest->mode = source->mode;

    // Copy the data pointers
    int j;
    for (j = 0; j < BLOCKS_PER_I_NODE; j++) {
        dest->pointer[j] = source->pointer[j];
    }
}

#endif //ASSIGNMENT_3_I_NODE_H
