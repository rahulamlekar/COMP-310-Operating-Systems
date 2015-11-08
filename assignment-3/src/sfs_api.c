#include <stdlib.h>
#include "sfs_api.h"
#include "constants.h"
#include "data_structures/super_block.h"
#include "data_structures/file_descriptor_table.h"
#include "disk_emu.h"
#include "data_structures/free_bitmap.h"
#include "data_structures/directory_cache.h"
#include "data_structures/disk_block_cache.h"
#include "data_structures/i_node_table.h"
#include "helpers/disk_access.h"


//InMemoryFileSystem* inMemoryFileSystem;
DirectoryCache* directoryCache;
DiskBlockCache* diskBlockCache;
FileDescriptorTable* fileDescriptorTable;
INodeTable* iNodeTable;

FreeBitMap* freeBitMap;

void allocate_in_memory_file_system() {
    // Allocate the memory for the in memory file system
    directoryCache = malloc(sizeof(DirectoryCache));
    diskBlockCache = malloc(sizeof(DiskBlockCache));
    fileDescriptorTable = malloc(sizeof(FileDescriptorTable));
    iNodeTable = malloc(sizeof(INodeTable));
    freeBitMap = malloc(sizeof(FreeBitMap));

    // Allocate empty disk data cache
    int i;
    for (i = 0; i < DISK_BLOCK_CACHE_SIZE; i++) {
        diskBlockCache->data[i] = malloc(DISK_BLOCK_SIZE);
    }
}



/**
 * creates the file system
 */
void mksfs(int fresh) {
    // Allocate all the necessary memory
    allocate_in_memory_file_system();

	//Implement mksfs here
    if (fresh) {
        // We need to construct the file system from scratch

        // Initialize a new fresh disk
        init_fresh_disk(FILE_SYSTEM_NAME, DISK_BLOCK_SIZE, DISK_BLOCK_COUNT);

        // Write superblock to disk
        SuperBlock* sb = malloc(sizeof(SuperBlock));
        sb->magic = SB_MAGIC;
        sb->block_size = 512;
        sb->file_system_size = DISK_BLOCK_COUNT;
        sb->i_node_table_length = I_NODE_COUNT;
        sb->root_directory_i_node = 0;

        write_blocks(0, 5, sb);

        // Write iNode table to disk

        write_blocks(I_NODE_TABLE_BLOCK_INDEX, 5, &iNodeTable);

    } else {
        // File system already exists on disk, so we need to load it from the disk.

        // Initialize an existing disk
        init_disk(FILE_SYSTEM_NAME, DISK_BLOCK_SIZE, DISK_BLOCK_COUNT);

        // Copy iNodes into local memory
        INode* tempINode;
        int i;
        for (i = 0; i < I_NODE_COUNT; i++) {
            // Temporarily copy the iNode off disk
            tempINode = load_i_node_from_disk(i);

            // Copy the data from the temp INode into memory
            INode_copy(&iNodeTable->i_node[i], tempINode);

            // Clear the temp data
            free(tempINode);
        }
    }
	return;
}


int directoryIndex = 0;

/**
 * get the name of the next file in directory
 */
int sfs_getnextfilename(char *fname) {
    // copies the name of the next file in the directory into fname
    // and returns non zero if there is a new file. Once all the files
    // have been returned, this function returns 0.

    if (directoryIndex < 16) {
        // Copy the
        strcpy(fname, directoryCache->directory[directoryIndex].name);
        // Increase the index for next time
        directoryIndex++;
        return 1;
    } else {
        // We have finished looping through the directory, so return 0
        return 0;
    }
}


/**
 * get the size of the given file
 */
int sfs_getfilesize(const char* path) {
    // Loop through the directory cache and check the names
    int i;
    for (i = 0; i < 5; i++) {
        if (strcmp(directoryCache->directory[i].name, path) == 0) {
            // It's the same name, so figure out the file size
            int iNodeIndex = directoryCache->directory[i].i_node_index;
            return iNodeTable->i_node[iNodeIndex].size;
        }
    }

    // There is none, so return error
	return -1;
}


/**
 * opens the given file
 */
int sfs_fopen(char *name) {
    // Get an open FDTable index
    int newIndex = FileDescriptorTable_getOpenIndex(*fileDescriptorTable);
    // If there is no space, return error
    if (newIndex == -1) {
        return -1;
    }

    // There is space, so let's fill up the spot on the table
    fileDescriptorTable->fd[newIndex].i_node_number = 0; // Set the iNode
    fileDescriptorTable->fd[newIndex].read_write_pointer = 0;

	return newIndex;
}


/**
 * closes the given file
 */
int sfs_fclose(int fileID){
    // Mark spot empty on fd table
    FileDescriptorTable_markClosed(fileDescriptorTable, fileID);

	return 0;
}


/**
 * read characters from disk into buf
 */
int sfs_fread(int fileID, char *buf, int length){

	//Implement sfs_fread here

    FileDescriptor fd = fileDescriptorTable->fd[fileID];
    INode iNode = iNodeTable->i_node[fd.i_node_number];
    int rwPointer = fd.read_write_pointer;

    int numBlocks = length / DISK_BLOCK_SIZE;

    int i;
    for (i = 0; i < numBlocks; i++) {
        // Extract the data from the disk
        void* data = DiskBlockCache_getData(diskBlockCache, iNode.pointer[rwPointer + i]);
        // Copy the data onto the buffer
        memcpy(buf + i, data, DISK_BLOCK_SIZE);
    }

	return 0;
}


/**
 * write buf characters into disk
 */
int sfs_fwrite(int fileID, const char *buf, int length){

    // Get the iNode of the desired file
    FileDescriptor fileDescriptor = fileDescriptorTable->fd[fileID];
    INode* fileINode = &iNodeTable->i_node[fileDescriptor.i_node_number];

	//Implement sfs_fwrite here

    int numBlocks = length / DISK_BLOCK_SIZE;
    int startingBlockIndex = fileDescriptor.read_write_pointer;
    int endingWRPointer = fileDescriptor.read_write_pointer + numBlocks;

    int i;
    for (i = startingBlockIndex; i <= endingWRPointer; i++) {
        // Get an index for the new data
        int newBlockDiskIndex = FreeBitMap_getFreeBitAndMarkUnfree(freeBitMap);

        // Save the new index to the iNode
        fileINode->pointer[i] = newBlockDiskIndex;

        // Write one data to the new location
        write_blocks(newBlockDiskIndex, 1, buf);
    }

    // Save the updated iNode to disk
    save_i_node_to_disk(fileDescriptor.i_node_number, fileINode);

    // Save the free bitmap to disk with updated info
    save_free_bitmap_to_disk(freeBitMap);

	return 0;
}


/**
 * seek to the location from beginning
 */
int sfs_fseek(int fileID, int loc){
    // Modify the pointer in memory.  Nothing to be done on disk!

    // I think it's just a matter of setting the read write pointer?
    fileDescriptorTable->fd[fileID].read_write_pointer = loc;

	return 0;
}


/**
 * removes a file from the filesystem
 */
int sfs_remove(char *file) {
    // First, grab the iNode and delete it's data blocks from memory
    int i;
    for (i = 0; i < 10; i++) {
        if (strcmp(directoryCache->directory[i].name, file) == 0) {
            // We found it
            break;
        }
    }

    int iNodeIndex = directoryCache->directory[i].i_node_index;

    INode iNode = iNodeTable->i_node[iNodeIndex];
    for (i = 0; i < BLOCKS_PER_I_NODE; i++) {
        // Erase the block from the disk
        erase_disk_block(iNode.pointer[i]);
        // Mark block free
        FreeBitMap_markBitFree(freeBitMap, iNode.pointer[i]);
    }


    // Then, delete the iNode from disk
    delete_i_node_from_disk(iNodeIndex);

    // Then, remove the file descriptor from memory
    // TODO

    // Then, save the updated free bitmap to disk
    save_free_bitmap_to_disk(freeBitMap);

	return 0;
}


int size_to_blocks(int size) {
    return size / DISK_BLOCK_SIZE;
}