//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_DIRECTORY_CACHE_H
#define ASSIGNMENT_3_DIRECTORY_CACHE_H

#include <string.h>
#include "directory.h"

#define DIRECTORY_CACHE_SIZE 16

typedef struct directory_cache {
    Directory directory[DIRECTORY_CACHE_SIZE];
} DirectoryCache;

int DirectoryCache_getDirectory(DirectoryCache cache, char name[]) {
    int i;
    for (i = 0; i < DIRECTORY_CACHE_SIZE; i++) {
        // Return pointer to iNode that matches
        if (strcmp(cache.directory[i].name, name) == 0) {
            return cache.directory[i].i_node_index;
        }
    }

    // Not Found, error state
    return NULL;
}

#endif //ASSIGNMENT_3_DIRECTORY_CACHE_H
