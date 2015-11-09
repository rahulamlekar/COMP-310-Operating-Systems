//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_DISK_BLOCK_CACHE_H
#define ASSIGNMENT_3_DISK_BLOCK_CACHE_H

typedef struct disk_block_cache {
    int indices[DISK_BLOCK_CACHE_SIZE];
    void* data[DISK_BLOCK_CACHE_SIZE];
    int writeIndex;


    int open[DISK_BLOCK_CACHE_SIZE];
} DiskBlockCache;


void read_data_block(int index, void* buffer) {
    read_blocks(DATA_BLOCK_TABLE_INDEX + index, 1, buffer);
}
void write_data_block(int index, void* buffer) {
    write_blocks(DATA_BLOCK_TABLE_INDEX + index, 1, buffer);
}

int DiskBlockCache_getCacheIndex(DiskBlockCache cache, int diskBlockId) {
    int i;
    for (i = 0; i < DISK_BLOCK_CACHE_SIZE; i++) {
        if (cache.indices[i] == diskBlockId) {
            return i;
        }
    }
    // Doesn't exist
    return -1;
}
void DiskBlockCache_markOpen(DiskBlockCache* table, int fileId) {
    table->open[fileId] = 0;
}
void DiskBlockCache_markClosed(DiskBlockCache* table, int fileId) {
    table->indices[fileId] =
    table->open[fileId] = 1;
}
int DiskBlockCache_isOpen(DiskBlockCache table, int fileId) {
    return table.open[fileId] == 0;
}
int DiskBlockCache_getOpenIndex(DiskBlockCache table) {
    int i;
    for (i = 0; i < FILE_DESCRIPTOR_TABLE_SIZE; i++) {
        if (DiskBlockCache_isOpen(table, i)) {
            return i;
        }
    }

    // No spot open... error
    return -1;
}

/**
 * Get a particular data block from the disk.  If it's in memory, use it.
 * Otherwise, read from disk and put in memory
 */
void* DiskBlockCache_getData(DiskBlockCache* cache, int diskId) {
    // Get index if existing
    int index = DiskBlockCache_getCacheIndex(*cache, diskId);
    if (index != -1) {
        //printf("Returning disk cache index %d\n", index);
        // It exists, so return the data!
        return cache->data[index];
    } else {
        // The data is not currently in the cache.  So, we will load it in from disk
        //printf("Data not in cache.  Loading from disk.\n");

        //printf("A\n");

        // Read the data from disk into that index
        read_data_block(diskId, cache->data[cache->writeIndex]);

        //printf("B\n");

        // Save index mapping
        cache->indices[cache->writeIndex] = diskId;

        void* output = cache->data[cache->writeIndex];

        //printf("C\n");

        // Advance the circular buffer, etc...
        cache->writeIndex = (cache->writeIndex + 1) % DISK_BLOCK_CACHE_SIZE;

        return output;
    }
}


#endif //ASSIGNMENT_3_DISK_BLOCK_CACHE_H
