//
// Created by Andrew on 2015-11-05.
//

#ifndef ASSIGNMENT_3_DISK_BLOCK_CACHE_H
#define ASSIGNMENT_3_DISK_BLOCK_CACHE_H

typedef struct disk_block_cache {
    int indices[DISK_BLOCK_CACHE_SIZE];
    void* data[DISK_BLOCK_CACHE_SIZE];
    //int writeIndex;


    int open[DISK_BLOCK_CACHE_SIZE];
} DiskBlockCache;


/**
 * Get a particular data block from the disk.  If it's in memory, use it.
 * Otherwise, read from disk and put in memory
 */
void* DiskBlockCache_getData(DiskBlockCache* cache, int diskId) {
    int index = -1;
    // Get index if existing
    index = DiskBlockCache_getCacheIndex(*cache, diskId);
    if (index != -1) {
        // It exists, so return the data!
        return cache->data[index];
    } else {
        // The data is not currently in the cache.  So, we will load it in from disk

        // Get an open index
        int cacheIndex = DiskBlockCache_getOpenIndex(*cache);
        // TODO: Implement functionality if no space left

        // Read the data from disk into that index
        read_data_block(diskId, cache->data[cacheIndex]);

        // Mark full
        DiskBlockCache_markClosed(cache, cacheIndex);
        // Save index mapping
        cache->indices[cacheIndex] = diskId;

        return cache->data[cacheIndex];
    }
}

void read_data_block(int index, void* buffer) {
    read_blocks(DATA_BLOCK_TABLE_INDEX + index, 1, buffer);
}
void write_data_block(int index, void* buffer) {
    write_blocks(DATA_BLOCK_TABLE_INDEX + index, 1, buffer);
}

///**
// * Read a data data from disk into the memory cache.  Returns cache index of data.
// */
//int read_from_disk_into_cache(Diskint diskBlockId) {
//    // First, find an empty index from the buffer to read into
//    int cacheIndex = DiskBlockCache_getOpenIndex(*diskBlockCache);
//
//    // Read the data from disk into the cache index
//    read_data_block(diskBlockId, diskBlockCache->data[cacheIndex]);
//    // Save the disk id for saving later
//    diskBlockCache->onDiskId[cacheIndex] = diskBlockId;
//
//    // Mark the new index as full
//    DiskBlockCache_markClosed(diskBlockCache, cacheIndex);
//
//    // Return the new cache index
//    return cacheIndex;
//}

/*
 * Helper functions
 */
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
    table->open[fileId] = 1;
}
void DiskBlockCache_markClosed(DiskBlockCache* table, int fileId) {
    table->indices[fileId] =
    table->open[fileId] = 0;
}
int DiskBlockCache_isOpen(DiskBlockCache table, int fileId) {
    return table.open[fileId];
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


#endif //ASSIGNMENT_3_DISK_BLOCK_CACHE_H
