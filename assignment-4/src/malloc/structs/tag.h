//
// Created by Andrew on 2015-11-23.
//

#ifndef ASSIGNMENT_4_TAG_H
#define ASSIGNMENT_4_TAG_H

#include <stddef.h>
#include "../utils/constants.h"

const size_t TAG_SIZE = sizeof(char);

int Block_prevIsFree(void* block) {
    char tagValue = *((char*) block - TAG_SIZE);
    return tagIsFree(tagValue);
}

int tagIsFree(char tagValue) {
    if (tagValue == TAG_IS_FREE) {
        return 1;
    } else if (tagValue == TAG_IS_NOT_FREE) {
        return 0;
    } else {
        return -1;
    }
}

#endif //ASSIGNMENT_4_TAG_H
