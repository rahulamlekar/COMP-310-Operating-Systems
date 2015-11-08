//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_FILE_DESCRIPTOR_H
#define ASSIGNMENT_3_FILE_DESCRIPTOR_H

typedef struct file_descriptor {
    int i_node_number;        // Corresponds to the file
    int read_write_pointer;   // A data data number
} FileDescriptor;

#endif //ASSIGNMENT_3_FILE_DESCRIPTOR_H
