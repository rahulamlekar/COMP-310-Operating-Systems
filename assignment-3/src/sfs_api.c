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


//InMemoryFileSystem* inMemoryFileSystem;
DirectoryCache* directoryCache;
DiskBlockCache* diskBlockCache;
FileDescriptorTable* fileDescriptorTable;
INodeTable* iNodeTable;

FreeBitMap* freeBitMap;


/**
 * creates the file system
 */
void mksfs(int fresh) {
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
        //init_disk(FILE_SYSTEM_NAME, DISK_BLOCK_SIZE, DISK_BLOCK_COUNT);

        // Load disk data into on memory file system

    }
	return;
}


/**
 * get the name of the next file in directory
 */
int sfs_getnextfilename(char *fname) {

	//Implement sfs_getnextfilename here	
	return 0;
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
            return directoryCache->directory[i].i_node->size;
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

	//Implement sfs_fclose here

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

    // Loop until i is at the corrent place
    int i;
    for (i = 0; i < BLOCKS_PER_I_NODE; i++) {
        if (rwPointer == iNode.pointer[i]) {
            break;
        }
    }

    // Ok, we know where we are in the iNode.  We will now copy the rest
    // into the buffer
    void* data;
    while (i < BLOCKS_PER_I_NODE) {
        // Get the data from the cache
        data = DiskBlockCache_getData(diskBlockCache, iNode.pointer[i]);
        // Copy the data into the buffer
        memcpy(buf, data, DISK_BLOCK_SIZE);

        i++;
    }

	return 0;
}


/**
 * write buf characters into disk
 */
int sfs_fwrite(int fileID, const char *buf, int length){

    // Get the iNode of the desired file
    FileDescriptor fileDescriptor = fileDescriptorTable->fd[fileID];
    INode fileINode = iNodeTable->i_node[fileDescriptor.i_node_number];

	//Implement sfs_fwrite here

    int numBlocks = length / DISK_BLOCK_SIZE;
    int startingSizeInBlocks = 0;
    int startingBlockIndex = 0;

    for (int i = 0; i < numBlocks; i++) {
        if ((i + startingBlockIndex) > startingBlockIndex) {
            // We need to make a new data

            // Get an index for the new data
            int newBlockDiskIndex = FreeBitMap_getFreeBitAndMarkUnfree(freeBitMap);

            // Write one data to the new location
            write_blocks(newBlockDiskIndex, 1, buf);
        } else {
            // We are going to overwrite a previously existing data

            // TODO: Get the previous index, etc...
            int previousIndex = 123;

            write_blocks(previousIndex, 1, buf);
        }
    }

	return 0;
}


/**
 * seek to the location from beginning
 */
int sfs_fseek(int fileID, int loc){
    // Modify the pointer in memory.  Nothing to be done on disk!



	return 0;
}


/**
 * removes a file from the filesystem
 */
int sfs_remove(char *file) {

	//Implement sfs_remove here	
	return 0;
}


/*
 * Disk Access Helper functions
 */

/**x
 * Read the nth data data into a buffer from disk
 */
INode* read_inode_data_block(int index) {
    INode* output = malloc(sizeof(INode));
    read_blocks(I_NODE_TABLE_BLOCK_INDEX + index, 1, output);
    return output;
}

int size_to_blocks(int size) {
    return size / DISK_BLOCK_SIZE;
}


void save_i_node_to_disk(int iNodeIndex) {
    // We will make a copy of the in-memory iNode with a few modifications

    INode* toDiskInode = malloc(sizeof(INode));



    // Free the copy
    free(toDiskInode);
}