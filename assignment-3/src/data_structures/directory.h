//
// Created by Andrew on 2015-11-06.
//

#ifndef ASSIGNMENT_3_DIRECTORY_H
#define ASSIGNMENT_3_DIRECTORY_H

#include "../constants.h"
#include "i_node.h"

typedef struct directory {
    char name[TOTAL_FILE_NAME_LENGTH+1];
    int i_node_index;
} Directory;

int isFileNameValid(char* name) {
    // Check main part of name
    int i = 0;
    while (name[i] != '.') {
        if (i >= FILE_NAME_LENGTH) {
            return -1;
        }
        i++;
    }
    // We hit the period, so advance
    int j = 0;
    while (name[i + 1 + j] != '\0') {
        if (j >= FILE_EXTENSION_LENGTH) {
            return -1;
        }
        j++;
    }

    return 1;
}

#endif //ASSIGNMENT_3_DIRECTORY_H
