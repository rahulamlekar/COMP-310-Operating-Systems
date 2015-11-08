//
// Created by Andrew on 2015-11-06.
//

#ifndef ASSIGNMENT_3_DIRECTORY_H
#define ASSIGNMENT_3_DIRECTORY_H

#include "../constants.h"
#include "i_node.h"

typedef struct directory {
    char name[FILE_NAME_LENGTH];
    INode* i_node;
} Directory;

#endif //ASSIGNMENT_3_DIRECTORY_H
