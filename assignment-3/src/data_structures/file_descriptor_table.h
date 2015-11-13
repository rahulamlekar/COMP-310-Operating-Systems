//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_FILE_DESCRIPTOR_TABLE_H
#define ASSIGNMENT_3_FILE_DESCRIPTOR_TABLE_H

#include "file_descriptor.h"
#include "../constants.h"

typedef struct file_descriptor_table {
    FileDescriptor fd[FILE_DESCRIPTOR_TABLE_SIZE];
    char in_use[FILE_DESCRIPTOR_TABLE_SIZE];
} FileDescriptorTable;

void FileDescriptorTable_markNotInUse(FileDescriptorTable *table, int fileId) {
    table->in_use[fileId] = 0;
}
void FileDescriptorTable_markInUse(FileDescriptorTable *table, int fileId) {
    table->in_use[fileId] = 1;
}
int FileDescriptorTable_isNotInUse(FileDescriptorTable table, int fileId) {
    return table.in_use[fileId] == 0;
}
int FileDescriptorTable_getOpenIndex(FileDescriptorTable table) {
    int i;
    for (i = 0; i < FILE_DESCRIPTOR_TABLE_SIZE; i++) {
        if (FileDescriptorTable_isNotInUse(table, i)) {
            return i;
        }
    }

    // No spot in_use... error
    return -1;
}
int FileDescriptorTable_getIndexOfInode(FileDescriptorTable table, int iNodeIndex) {
    int i;
    for (i = 0; i < FILE_DESCRIPTOR_TABLE_SIZE; i++) {
//        printf("Checking for iNode %d\n", iNodeIndex);
//        printf("TEEEEST: %d\n", table.fd[i].i_node_number);
        if (table.fd[i].i_node_number == iNodeIndex) {
            return i;
        }
    }

    // Error
    return -1;
}

void FileDescriptorTable_print(FileDescriptorTable table) {
    int i;
    for (i = 0; i < FILE_DESCRIPTOR_TABLE_SIZE; i++) {
        printf("FileDescriptor: {i_node_number: %d, read_write_pointer: %d}, in_use: %d\n", table.fd[i].i_node_number, table.fd[i].read_write_pointer, table.in_use[i]);
    }
}


#endif //ASSIGNMENT_3_FILE_DESCRIPTOR_TABLE_H
