//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_FILE_DESCRIPTOR_TABLE_H
#define ASSIGNMENT_3_FILE_DESCRIPTOR_TABLE_H

#include "file_descriptor.h"
#include "../constants.h"

typedef struct file_descriptor_table {
    FileDescriptor fd[FILE_DESCRIPTOR_TABLE_SIZE];
} FileDescriptorTable;

#endif //ASSIGNMENT_3_FILE_DESCRIPTOR_TABLE_H
