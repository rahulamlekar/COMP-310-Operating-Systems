//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_FREE_BITMAP_H
#define ASSIGNMENT_3_FREE_BITMAP_H

#include "../constants.h"

typedef struct free_bitmap {
    unsigned int bit[DISK_BLOCK_COUNT];
} FreeBitMap;


/**
 * Mark a particular index as unfree
 */
void FreeBitMap_markBitUnfree(FreeBitMap* map, int index) {
    map->bit[index] = 1;
}


/**
 * Mark a particular index as free
 */
void FreeBitMap_markBitFree(FreeBitMap* map, int index) {
    map->bit[index] = 0;
}


/**
 * Check if particular index is free
 */
int FreeBitMap_isBitFree(FreeBitMap map, int index) {
    return map.bit[index] == 0;
}


/**
 * Get the first free index
 */
int FreeBitMap_getFreeBit(FreeBitMap map) {
    int i;
    for (i = 0; i < DISK_BLOCK_COUNT; i++) {
        if (FreeBitMap_isBitFree(map, i)) {
            return i;
        }
    }

    return -1;
}


/**
 * Find the first free index.  Mark it as unfree and return the index.
 */
int FreeBitMap_getFreeBitAndMarkUnfree(FreeBitMap* map) {
    int output = FreeBitMap_getFreeBit(*map);
    FreeBitMap_markBitUnfree(map, output);
    return output;
}

#endif //ASSIGNMENT_3_FREE_BITMAP_H
