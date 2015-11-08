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

#endif //ASSIGNMENT_3_I_NODE_H
