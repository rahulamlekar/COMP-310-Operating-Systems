//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_DIRECTORY_CACHE_H
#define ASSIGNMENT_3_DIRECTORY_CACHE_H

#include <string.h>
#include "directory.h"

typedef struct directory_cache {
    Directory directory[I_NODE_COUNT];
    char open[I_NODE_COUNT]; // Char uses fewer bits than int
    int readIndex;
} DirectoryCache;

void DirectoryCache_markOpen(DirectoryCache* table, int fileId) {
    table->open[fileId] = 0;
}
void DirectoryCache_markClosed(DirectoryCache* table, int fileId) {
    table->open[fileId] = 1;
}
int DirectoryCache_isOpen(DirectoryCache table, int fileId) {
    return table.open[fileId] == 0;
}
int DirectoryCache_getOpenIndex(DirectoryCache table) {
    int i;
    for (i = 0; i < I_NODE_COUNT; i++) {
        if (DirectoryCache_isOpen(table, i)) {
            return i;
        }
    }

    // No spot open... error
    return -1;
}

void DirectoryCache_deleteIndex(DirectoryCache* cache, int index) {
    // Mark it open
    DirectoryCache_markOpen(cache, index);
    // Delete the data
    cache->directory[index].i_node_index = 0;
    strcpy(cache->directory[index].name, "");

    // Advance the readindex if possible
    if (cache->readIndex == index) {
        cache->readIndex = (cache->readIndex + 1) % I_NODE_COUNT;
    }
}

int DirectoryCache_getDirectoryINodeIndex(DirectoryCache* cache, char *name) {
    int i;
    for (i = 0; i < I_NODE_COUNT; i++) {
//        printf("Directory {name: \"%s\", inode: %d}\n", cache->directory[i].name, cache->directory[i].i_node_index);

        // Return pointer to iNode that matches
        if (strcmp(cache->directory[i].name, name) == 0) {
//            printf("%s Returning: %d from index %d\n", cache->directory[i].name, cache->directory[i].i_node_index, i);
            return cache->directory[i].i_node_index;
        }
    }

    // Not Found, error state
    return -1;
}

void DirectoryCache_print(DirectoryCache cache) {
    int i;
    for (i = 0; i < I_NODE_COUNT; i++) {
        printf("Directory: {name: \"%s\", i_node_index: %d } open: %d\n", cache.directory[i].name, cache.directory[i].i_node_index, DirectoryCache_isOpen(cache, i));
    }
}

#endif //ASSIGNMENT_3_DIRECTORY_CACHE_H
