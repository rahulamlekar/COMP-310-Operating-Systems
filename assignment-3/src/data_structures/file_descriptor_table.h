//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_FILE_DESCRIPTOR_TABLE_H
#define ASSIGNMENT_3_FILE_DESCRIPTOR_TABLE_H

#include "file_descriptor.h"
#include "../constants.h"

typedef struct file_descriptor_table {
    FileDescriptor fd[FILE_DESCRIPTOR_TABLE_SIZE];
    int open[FILE_DESCRIPTOR_TABLE_SIZE];
} FileDescriptorTable;

void FileDescriptorTable_markOpen(FileDescriptorTable* table, int fileId) {
    table->open[fileId] = 0;
}
void FileDescriptorTable_markClosed(FileDescriptorTable* table, int fileId) {
    table->open[fileId] = 1;
}
int FileDescriptorTable_isOpen(FileDescriptorTable table, int fileId) {
    return table.open[fileId] == 0;
}
int FileDescriptorTable_getOpenIndex(FileDescriptorTable table) {
    int i;
    for (i = 0; i < FILE_DESCRIPTOR_TABLE_SIZE; i++) {
        if (FileDescriptorTable_isOpen(table, i)) {
            return i;
        }
    }

    // No spot open... error
    return -1;
}
int FileDescriptorTable_getIndexOfInode(FileDescriptorTable table, int iNodeIndex) {
    int i;
    for (i = 0; i < FILE_DESCRIPTOR_TABLE_SIZE; i++) {
        printf("Checking for iNode %d\n", iNodeIndex);
        printf("TEEEEST: %d\n", table.fd[i].i_node_number);
        if (table.fd[i].i_node_number == iNodeIndex) {
            return i;
        }
    }

    // Error
    return -1;
}


#endif //ASSIGNMENT_3_FILE_DESCRIPTOR_TABLE_H
