#include <stdlib.h>
#include <stdio.h>
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
#include "data_structures/indirect_block_pointer.h"


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
    printf("Indirect Block Pointer Size: %d\n", sizeof(IndirectBlockPointer));
    printf("Directory cache size: %d\n", sizeof(DirectoryCache));
    printf("INodeTable size: %d\n", sizeof(INodeTable));
    printf("FreeBitMap size: %d\n", sizeof(FreeBitMap));
    printf("INode Size: %d\n", sizeof(INode));

    // Allocate all the necessary memory
    allocate_in_memory_file_system();

    printf("Starting mksfs()\n");

	//Implement mksfs here
    if (fresh) {
        // We need to construct the file system from scratch

        // Delete the disk file, if it exists
        int test = remove(FILE_SYSTEM_NAME);
//        printf("Test: %d\n", test);

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

        // Write the rest of the stuff to disk
        save_local_file_system_to_disk(freeBitMap, iNodeTable, directoryCache);

    } else {
        // File system already exists on disk, so we need to load it from the disk.

        // Initialize an existing disk
        init_disk(FILE_SYSTEM_NAME, DISK_BLOCK_SIZE, DISK_BLOCK_COUNT);

        // Copy iNodes into local memory
        iNodeTable = load_i_node_cache_from_disk();
        // Copy directory cache into memory
        directoryCache = load_directory_cache_from_disk();
        // Copy the free bit map into memory
        freeBitMap = load_free_bitmap_from_disk();

//        FreeBitMap_print(*freeBitMap);

//        exit(1);
    }
	return;
}


/**
 * copies the name of the next file in the directory into fname
 * and returns non zero if there is a new file. Once all the files
 * have been returned, this function returns 0.
 */
int sfs_getnextfilename(char *fname) {

    int index = directoryCache->readIndex;
    if (index < I_NODE_COUNT) {
        // Copy the
        strcpy(fname, directoryCache->directory[index].name);
        // Increase the index for next time
        directoryCache->readIndex++;
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
    printf("Opening %s\n", name);

    FileDescriptorTable_print(*fileDescriptorTable);

//    printf("Opening %s\n", name);
    // Get the iNode index from the Directory cache
    int iNodeIndex = DirectoryCache_getDirectoryINodeIndex(directoryCache, name);

    printf("First INodeIndex: %d\n", iNodeIndex);

    if (iNodeIndex != -1) {
        printf("test\n");
        // The iNode already exists.  So, let's see if it's already open
        int existingFdIndex = FileDescriptorTable_getIndexOfInode(*fileDescriptorTable, iNodeIndex);
        printf("existingFdIndex: %d\n", existingFdIndex);
        if (existingFdIndex != -1) {
//            printf("Already exists!!!\n");
            // Make sure that it's marked in use
            FileDescriptorTable_markInUse(fileDescriptorTable, existingFdIndex);
            printf("%s already open.  Returning fd: %d\n", name, existingFdIndex);
            // The file is already in_use!!!!!!  So we just return it's FD.
            return existingFdIndex;
        }
    }

    // Add an entry to the in_use FD table
    int fdIndex = FileDescriptorTable_getOpenIndex(*fileDescriptorTable);

    printf("fdIndex: %d\n", fdIndex);

    // Mark the new index as closed
    FileDescriptorTable_markInUse(fileDescriptorTable, fdIndex);

    if (iNodeIndex == -1) {
        printf("CREATING BRAND NEW FILE.\n");
        // Getting the iNode index failed.  So, the file doesn't exist!  We must create
        // a new file!

        // Get an empty index on the iNode table
        iNodeIndex = INodeTable_getOpenIndex(*iNodeTable);
        printf("New iNode Index: %d\n", iNodeIndex);

        // Get the iNode and give it blank data!
        INode* iNode = &iNodeTable->i_node[iNodeIndex];
        INode_new(iNode);

        // Mark the iNode space full
        INodeTable_markClosed(iNodeTable, iNodeIndex);

        // Write the iNode to the hard drive
//        save_i_node_to_disk(iNodeIndex, iNode);

        DirectoryCache_print(*directoryCache);

        // Make a directory object and add it to the disk
        int directoryIndex = DirectoryCache_getOpenIndex(*directoryCache);

        if (directoryIndex == -1) {
            // Error.  Too many directories in file system!!!
            return -1;
        }

        DirectoryCache_markClosed(directoryCache, directoryIndex);
        printf("Directory object for %s has iNodeIndex %d\n", name, iNodeIndex);
        printf("Before: %d\n", iNodeIndex);
        directoryCache->directory[directoryIndex].i_node_index = (int) iNodeIndex;
        printf("After: %d\n", directoryCache->directory[directoryIndex].i_node_index);

        printf("Test directoryIndex: %d\n", directoryIndex);
        printf("Currentname: %s\n", directoryCache->directory[directoryIndex].name);
        // Copy the name
        strcpy(directoryCache->directory[directoryIndex].name, name);
        printf("Complete strcpy()\n");
        // TODO: Copy directory to disk
    }

    printf("iNodeIndex: %d\n", iNodeIndex);

    // Copy the iNode index to the table value
    fileDescriptorTable->fd[fdIndex].i_node_number = iNodeIndex;
    fileDescriptorTable->fd[fdIndex].read_write_pointer = 0;
    // Maybe set it to the size??? //TODO
    fileDescriptorTable->fd[fdIndex].read_write_pointer = iNodeTable->i_node[iNodeIndex].size;

    printf("FDINDEX: %d\n", fdIndex);

    // Save our cached version of the file system
    save_local_file_system_to_disk(freeBitMap, iNodeTable, directoryCache);

    // Return the index from the in_use file table
	return fdIndex;
}


/**
 * closes the given file
 */
int sfs_fclose(int fileID){
    FileDescriptorTable_print(*fileDescriptorTable);

    // Error if already closed
    if (FileDescriptorTable_isNotInUse(*fileDescriptorTable, fileID)) {
        return -1;
    }

    // Mark spot empty on fd table
    FileDescriptorTable_markNotInUse(fileDescriptorTable, fileID);

    // Success!
	return 0;
}


/**
 * read characters from disk into buf
 */
int sfs_fread(int fileID, char *buf, int length){
    // Return if file is closed
    if (FileDescriptorTable_isNotInUse(*fileDescriptorTable, fileID)) {
        printf("File %d is closed.  Cannot read...\n", fileID);
        return 0;
    }

    printf("Reading %d bytes from file %d\n", length, fileID);

    FileDescriptor* fd = &fileDescriptorTable->fd[fileID];
    INode iNode = iNodeTable->i_node[fd->i_node_number];

    // Output is the total data that we'have read...
    int output = 0;

    int amountLeftToRead = length;

    while (amountLeftToRead > 0) {
        //printf("i: %d\n", i);

        printf("Current RW pointer %d\n", fd->read_write_pointer);

        int currentBlockIndex = fd->read_write_pointer / DISK_BLOCK_SIZE;
        int startingIndexOfBlock = fd->read_write_pointer % DISK_BLOCK_SIZE;

        int amountToRead;
        if (amountLeftToRead > (DISK_BLOCK_SIZE - startingIndexOfBlock)) {
            amountToRead = DISK_BLOCK_SIZE - startingIndexOfBlock;
        } else {
            amountToRead = amountLeftToRead;
        }

        // Extract the data from the disk

        // Allocate an empty block
        void* data = malloc(DISK_BLOCK_SIZE);
        // Fill the block with the disk data

        printf("File %d reading %d bytes from disk block %d\n", fileID, amountToRead, iNode.pointer[currentBlockIndex]);

        read_data_block(iNode.pointer[currentBlockIndex], data);

        // Copy the desired amount of memory into the output buffer
        memcpy(buf, data + startingIndexOfBlock, amountToRead);
        //printf("Finished %d\n", i);

        // Advance the rw pointer
        fd->read_write_pointer += amountToRead;

        // Record how much has been read and how much less there is to read now
        output += amountToRead;
        amountLeftToRead -= amountToRead;

        // Increment the buffer for the next loop
        buf += amountToRead;
    }

    //printf("Finished copying blocks...\n");

    printf("Finished reading %d bytes.\n", output);

	return output;
}


/**
 * write buf characters into disk
 */
int sfs_fwrite(int fileID, const char *buf, int length){

    printf("Start write procedure to write %d bytes to file %d\n", length, fileID);

    // Get the iNode of the desired file
    FileDescriptor* fileDescriptor = &fileDescriptorTable->fd[fileID];
    INode* fileINode = &iNodeTable->i_node[fileDescriptor->i_node_number];

    // Keep track of how much we have written, and how much we must write
    int totalBytesWritten = 0;
    int amountLeftToWrite = length;

    while (amountLeftToWrite > 0) {

        printf("Current RW pointer %d\n", fileDescriptor->read_write_pointer);

        int currentBlockIndex = fileDescriptor->read_write_pointer / DISK_BLOCK_SIZE;
        int startingIndexOfBlock = fileDescriptor->read_write_pointer % DISK_BLOCK_SIZE;

        int amountToWrite;
        if (amountLeftToWrite > (DISK_BLOCK_SIZE - startingIndexOfBlock)) {
            amountToWrite = DISK_BLOCK_SIZE - startingIndexOfBlock;
        } else {
            amountToWrite = amountLeftToWrite;
        }

//        printf("CurrentblockIndex = %d, startingIndexOfBlock: %d, amountLeftToWrite: %d, amountToWrite: %d\n",
//               currentBlockIndex, startingIndexOfBlock, amountLeftToWrite, amountToWrite);

        int newBlockDiskIndex;
        int blockIsNew;
        if (fileINode->pointer[currentBlockIndex] > 0) {
            // We're reusing a previously existing block
            newBlockDiskIndex = fileINode->pointer[currentBlockIndex];
            // It's an old block
            blockIsNew = 0;
        } else {
//            // Get a new block from the hard drive
            newBlockDiskIndex = FreeBitMap_getFreeBitAndMarkUnfree(freeBitMap);
            // It's a new block!
            blockIsNew = 1;
        }

        // Save the new index to the iNode
        fileINode->pointer[currentBlockIndex] = newBlockDiskIndex;

        // Create an empty buffer
        void* newBuffer = malloc(DISK_BLOCK_SIZE);

        // If the block is already partially full, then we need to load the old data
        if (!blockIsNew) {
            read_data_block(newBlockDiskIndex, newBuffer);
        }

        // Copy the desired amount of data onto it
        memcpy(newBuffer + startingIndexOfBlock, buf, amountToWrite);

        printf("File %d writing %d bytes to disk block %d\n", fileID, amountToWrite, newBlockDiskIndex);

        // Write the new buffer to the hard drive
        write_data_block(newBlockDiskIndex, newBuffer);

//        printf("File %d: Writing %d bytes to block %d.\n",fileID, amountToWrite, newBlockDiskIndex);

        // Save how much we have written to the file pointer
        fileDescriptor->read_write_pointer += amountToWrite;

        // Track how much we have written.  This will be returned to user
        totalBytesWritten += amountToWrite;
        amountLeftToWrite -= amountToWrite;

        // Increment the buffer
        buf += amountToWrite;

        // Record file size change
        if (fileDescriptor->read_write_pointer > fileINode->size) {
            fileINode->size = fileDescriptor->read_write_pointer;
        }
    }

    // Save the updated iNode to disk
//    save_i_node_to_disk(fileDescriptor->i_node_number, fileINode);

    // Save the free bitmap to disk with updated info
//    save_free_bitmap_to_disk(freeBitMap);

    // Save our cached version of the file system
    save_local_file_system_to_disk(freeBitMap, iNodeTable, directoryCache);

	return totalBytesWritten;
}


/**
 * seek to the location from beginning
 */
int sfs_fseek(int fileID, int loc){
    // Modify the pointer in memory.  Nothing to be done on disk!
    printf("File %d seeking to loc %d.\n", fileID, loc);

    // I think it's just a matter of setting the read write pointer?
    fileDescriptorTable->fd[fileID].read_write_pointer = loc;

	return 0;
}


/**
 * removes a file from the filesystem
 */
int sfs_remove(char *file) {
    printf("Begin removing %s.\n", file);

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


//    // Then, delete the iNode from disk
//    delete_i_node_from_disk(iNodeIndex);
//
//    // Then, remove the file descriptor from memory
//    // TODO
//
//    // Then, save the updated free bitmap to disk
//    save_free_bitmap_to_disk(freeBitMap);

    // Save our cached version of the file system
    save_local_file_system_to_disk(freeBitMap, iNodeTable, directoryCache);

    printf("Finished removing %s.\n", file);

	return 0;
}
