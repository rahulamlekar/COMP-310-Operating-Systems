//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_I_NODE_TABLE_H
#define ASSIGNMENT_3_I_NODE_TABLE_H

#include "i_node.h"
#include "../constants.h"

typedef struct i_node_table{
    INode i_node[I_NODE_COUNT];
    int open[I_NODE_COUNT];
} INodeTable;

void INodeTable_markOpen(INodeTable* table, int fileId) {
    table->open[fileId] = 0;
}
void INodeTable_markClosed(INodeTable* table, int fileId) {
    table->open[fileId] = 1;
}
int INodeTable_isOpen(INodeTable table, int fileId) {
    return table.open[fileId] == 0;
}
int INodeTable_getOpenIndex(INodeTable table) {
    int i;
    for (i = 0; i < I_NODE_COUNT; i++) {
        if (INodeTable_isOpen(table, i)) {
            return i;
        }
    }

    // No spot open... error
    return -1;
}

void INodeTable_deleteINode(INodeTable* table, int index) {
    // Mark the spot open
    INodeTable_isOpen(*table, index);
    // Clear the data
    INode_new(&table->i_node[index]);
}

void INodeTable_reset(INodeTable* table) {
    int i;
    for (i = 0; i < I_NODE_COUNT; i++) {
        INodeTable_deleteINode(table, i);
    }
}

#endif //ASSIGNMENT_3_I_NODE_TABLE_H
