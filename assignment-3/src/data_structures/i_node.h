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
    int ind_pointer;
    int pointer[BLOCKS_PER_I_NODE];
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

void INode_new(INode* iNode) {
    iNode->size = 0;
    iNode->gid = 0;
    iNode->uid = 0;
    iNode->ind_pointer = -1; // Initialize to -1 so that we know that there isn't one
    iNode->link_cnt = 0;
    iNode->mode = 0;

    int j;
    for (j = 0; j < BLOCKS_PER_I_NODE; j++) {
        iNode->pointer[j] = -1;
    }
}

void INode_print(INode iNode) {
    printf("INode {\n");
    printf("    mode: %d\n", iNode.mode);
    printf("    link_cnt: %d\n", iNode.link_cnt);
    printf("    uid: %d\n", iNode.uid);
    printf("    gid: %d\n", iNode.gid);
    printf("    size: %d\n", iNode.size);
    printf("    ind_pointer: %d\n", iNode.ind_pointer);
    int i;
    for (i = 0; i < BLOCKS_PER_I_NODE; i++) {
        printf("    pointer %d: %d\n", i, iNode.pointer[i]);
    }
    printf("}\n");
}

#endif //ASSIGNMENT_3_I_NODE_H
