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
#include "data_structures/indirect_block_pointer.h"

DirectoryCache* directoryCache = NULL;
FileDescriptorTable* fileDescriptorTable = NULL;
INodeTable* iNodeTable = NULL;
FreeBitMap* freeBitMap = NULL;

void allocate_necessary_memory() {
    if (fileDescriptorTable) {
        memset(fileDescriptorTable, '\0', sizeof(FileDescriptorTable));
    } else {
        fileDescriptorTable = malloc(sizeof(FileDescriptorTable));
    }
    if (directoryCache) {
        memset(directoryCache, '\0', sizeof(DirectoryCache));
    } else {
        directoryCache = malloc(sizeof(DirectoryCache));
    }
    if (iNodeTable) {
        memset(iNodeTable, '\0', sizeof(INodeTable));
    } else {
        iNodeTable = malloc(sizeof(INodeTable));
    }
    if (freeBitMap) {
        memset(freeBitMap, '\0', sizeof(FreeBitMap));
    } else {
        freeBitMap = malloc(sizeof(FreeBitMap));
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
    printf("SuberBlock size: %d\n", sizeof(SuperBlock));

    printf("Starting mksfs()\n");

    // Allocate the local file system
    allocate_necessary_memory();

    FileDescriptorTable_print(*fileDescriptorTable);
    DirectoryCache_print(*directoryCache);

    printf("mksfs step 1\n");

	//Implement mksfs here
    if (fresh) {
        // We need to construct the file system from scratch

        // Delete the disk file, if it exists
        remove(FILE_SYSTEM_NAME);

        // Initialize a new fresh disk
        init_fresh_disk(FILE_SYSTEM_NAME, DISK_BLOCK_SIZE, DISK_BLOCK_COUNT);

        // Write superblock to disk
        SuperBlock* sb = malloc(sizeof(SuperBlock));
        sb->magic = SB_MAGIC;
        sb->block_size = 512;
        sb->file_system_size = DISK_BLOCK_COUNT;
        sb->i_node_table_length = I_NODE_COUNT;
        sb->root_directory_i_node = 0;
        write_blocks(SUPER_BLOCK_INDEX, 1, sb);
        free(sb);

        // Write the rest of the stuff to disk
        save_local_file_system_to_disk(freeBitMap, iNodeTable, directoryCache);
    } else {
        // File system already exists on disk, so we need to load it from the disk.

        // Initialize an existing disk
        init_disk(FILE_SYSTEM_NAME, DISK_BLOCK_SIZE, DISK_BLOCK_COUNT);

        // Copy iNodes into local memory
        load_i_node_cache_from_disk(iNodeTable);
        // Copy directory cache into memory
        load_directory_cache_from_disk(directoryCache);
        // Copy the free bit map into memory
        load_free_bitmap_from_disk(freeBitMap);
    }

    FileDescriptorTable_print(*fileDescriptorTable);

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
    printf("Getting size of %s\n", path);
    DirectoryCache_print(*directoryCache);
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

    // Check that the name is valid
    if (isFileNameValid(name) == -1) {
        return -1;
    }

    FileDescriptorTable_print(*fileDescriptorTable);

    // Get the iNode index from the Directory cache
    int iNodeIndex = DirectoryCache_getDirectoryINodeIndex(directoryCache, name);

    printf("First INodeIndex: %d\n", iNodeIndex);

    if (iNodeIndex != -1) {
        printf("test\n");
        // The iNode already exists.  So, let's see if it's already open
        int existingFdIndex = FileDescriptorTable_getIndexOfInode(*fileDescriptorTable, iNodeIndex);
        printf("existingFdIndex: %d\n", existingFdIndex);
        if (existingFdIndex != -1) {
            // Make sure that it's marked in use
            FileDescriptorTable_markInUse(fileDescriptorTable, existingFdIndex);
            // The file is already in_use!!!!!!  So we just return it's FD.
            return existingFdIndex;
            //return -1;  // Can't open file twice
        }
    }

    // Add an entry to the in_use FD table
    int fdIndex = FileDescriptorTable_getOpenIndex(*fileDescriptorTable);

    if (fdIndex == -1) {
        // FD Table is full!
        printf("Cannot open file.  Too many files already open!\n");
        return -1;
    }

    printf("fdIndex: %d\n", fdIndex);

    // Mark the new index as closed
    FileDescriptorTable_markInUse(fileDescriptorTable, fdIndex);

    if (iNodeIndex == -1) {
        printf("CREATING BRAND NEW FILE.\n");
        // Getting the iNode index failed.  So, the file doesn't exist!  We must create
        // a new file!

        // Get an empty index on the iNode table
        iNodeIndex = INodeTable_getOpenIndex(*iNodeTable);

        // Get the iNode and give it blank data!
        INode* iNode = &iNodeTable->i_node[iNodeIndex];
        INode_new(iNode);

        // Mark the iNode space full
        INodeTable_markClosed(iNodeTable, iNodeIndex);

        DirectoryCache_print(*directoryCache);

        // Make a directory object and add it to the disk
        int directoryIndex = DirectoryCache_getOpenIndex(*directoryCache);

        if (directoryIndex == -1) {
            // Error.  Too many directories in file system!!!
            return -1;
        }

        DirectoryCache_markClosed(directoryCache, directoryIndex);
        directoryCache->directory[directoryIndex].i_node_index = iNodeIndex;
        // Copy the name
        strcpy(directoryCache->directory[directoryIndex].name, name);
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

//    // Error if already closed
    if (FileDescriptorTable_isNotInUse(*fileDescriptorTable, fileID)) {
        return -1;
    }

    // Mark spot empty on fd table
    FileDescriptorTable_markNotInUse(fileDescriptorTable, fileID);

    // Remove data from FD table
    fileDescriptorTable->fd[fileID].read_write_pointer = 0;
    fileDescriptorTable->fd[fileID].i_node_number = 0;

    // Success!
	return 0;
}


/**
 * read characters from disk into buf
 */
int sfs_fread(int fileID, char *buf, int length){
    printf("Reading %d bytes from file %d\n", length, fileID);

    // Return if file is closed
    if (FileDescriptorTable_isNotInUse(*fileDescriptorTable, fileID)) {
        printf("File %d is closed.  Cannot read...\n", fileID);
        return 0;
    }



    FileDescriptor* fd = &fileDescriptorTable->fd[fileID];
    INode iNode = iNodeTable->i_node[fd->i_node_number];

    printf("Reading iNode:\n");
    INode_print(iNode);

    // Output is the total data that we'have read...
    int output = 0;

    int amountLeftToRead;
    if ((fd->read_write_pointer + length + 1) > iNode.size) {
        printf("size: %d, rwpointer: %d\n", iNode.size, fd->read_write_pointer);
        // Read what's left of the file
        amountLeftToRead = iNode.size - fd->read_write_pointer;
    } else {
        // Read the whole file
        amountLeftToRead = length;
    }

    printf("AmountLeftToRead: %d\n", amountLeftToRead);

    // We mighth have to access the indirect block
    IndirectBlockPointer* indirectBlockPointer = NULL;

    // Allocate an empty block
    void* data = malloc(DISK_BLOCK_SIZE);

    while (amountLeftToRead > 0) {
        printf("Current RW pointer %d\n", fd->read_write_pointer);

        int currentBlockIndex = fd->read_write_pointer / DISK_BLOCK_SIZE;
        int startingIndexOfBlock = fd->read_write_pointer % DISK_BLOCK_SIZE;

        /*
         * Begin indirect block setup.
         */

        int indexWhereDataGoesInIndirectBLock = -1;
        // If the current block index is greater than 11, then we'll have to hit the indirect block
        if (currentBlockIndex >= BLOCKS_PER_I_NODE) {
            // Ok, we're hitting the indirect pointer.
            printf("Hitting indirect pointer\n");

            // This is the index within the indirect block
            indexWhereDataGoesInIndirectBLock = currentBlockIndex - BLOCKS_PER_I_NODE;

            // Now, we see if the iNode already has an indirect block.
            if (indirectBlockPointer) {
                // We already have an indirect block in memory, so we'll use it
                printf("IndirectBlockPointer already in memory.\n");
            } else {
                printf("Loading indirect block %d\n", iNode.ind_pointer);
                // There's no indirect block in memory
                indirectBlockPointer = malloc(DISK_BLOCK_SIZE);

                // Load the indirect block from disk
                read_data_block(iNode.ind_pointer, indirectBlockPointer);
            }

        }

        /*
         * End indirect block setup.
         */

        int amountToRead;
        if (amountLeftToRead > (DISK_BLOCK_SIZE - startingIndexOfBlock)) {
            amountToRead = DISK_BLOCK_SIZE - startingIndexOfBlock;
        } else {
            amountToRead = amountLeftToRead;
        }

        /*
         * Extract the data from the disk
         */
        // Empty the data buffer
        memset(data, '\0', DISK_BLOCK_SIZE);

        // Read data from the correct source
        printf("Test: %d\n", indexWhereDataGoesInIndirectBLock);
        if (indexWhereDataGoesInIndirectBLock > -1) {
            printf("File %d reading %d bytes indirectly from disk block %d\n", fileID, amountToRead, indirectBlockPointer->block[indexWhereDataGoesInIndirectBLock]);
            read_data_block(indirectBlockPointer->block[indexWhereDataGoesInIndirectBLock], data);
        } else {
            printf("File %d reading %d bytes from disk block %d\n", fileID, amountToRead, iNode.pointer[currentBlockIndex]);
            // Read direct from iNode
            read_data_block(iNode.pointer[currentBlockIndex], data);
        }

        // Copy the desired amount of memory into the output buffer
        memcpy(buf, data + startingIndexOfBlock, (size_t) amountToRead);

        // Advance the rw pointer
        fd->read_write_pointer += amountToRead;

        // Record how much has been read and how much less there is to read now
        output += amountToRead;
        amountLeftToRead -= amountToRead;

        // Increment the buffer for the next loop
        buf += amountToRead;
    }

    // Free the data block
    free(data);

    if (indirectBlockPointer) {
        free(indirectBlockPointer);
    }

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

    // We mighth have to access the indirect block
    IndirectBlockPointer* indirectBlockPointer = NULL;

    // Create an empty buffer
    void* newBuffer = malloc(DISK_BLOCK_SIZE);

    while (amountLeftToWrite > 0) {
        // Empty the contents of newBuffer
        memset(newBuffer, '\0', DISK_BLOCK_SIZE);

        int currentBlockIndex = fileDescriptor->read_write_pointer / DISK_BLOCK_SIZE;
        int startingIndexOfBlock = fileDescriptor->read_write_pointer % DISK_BLOCK_SIZE;

        /*
         * Begin indirect block setup
         */

        int isUsingIndirectBlock = currentBlockIndex >= BLOCKS_PER_I_NODE;
        int indexWhereDataGoesInIndirectBLock = -1;
        // If the current block index is greater than 11, then we'll have to hit the indirect block
        if (isUsingIndirectBlock) {
            // Ok, we're hitting the indirect pointer.
            printf("Hitting indirect pointer\n");

            // This is the index within the indirect block
            indexWhereDataGoesInIndirectBLock = currentBlockIndex - BLOCKS_PER_I_NODE;

            // Now, we see if the iNode already has an indirect block.
            if (indirectBlockPointer) {
                // We already have an indirect block in memory, so we'll use it
                printf("IndirectBlockPointer already in memory.\n");
            } else {
                // There's no indirect block in memory
                indirectBlockPointer = malloc(DISK_BLOCK_SIZE);

                // We either have to load the indirect block from disk, or create a new one
                if (fileINode->ind_pointer > 0) {
                    printf("File already has indirect pointer %d\n", fileINode->ind_pointer);
                    // One already exists, so we'll load the data
                    read_data_block(fileINode->ind_pointer, indirectBlockPointer);
                } else {
                    // Find an empty index to save it
                    fileINode->ind_pointer = FreeBitMap_getFreeBit(*freeBitMap);
                    FreeBitMap_markBitUnfree(freeBitMap, fileINode->ind_pointer);
                    printf("Reserved block %d for an indirect pointer.\n", fileINode->ind_pointer);
                }
            }

        }

        /*
         * End indirect block setup
         */

        int amountToWrite;
        if (amountLeftToWrite > (DISK_BLOCK_SIZE - startingIndexOfBlock)) {
            amountToWrite = DISK_BLOCK_SIZE - startingIndexOfBlock;
        } else {
            amountToWrite = amountLeftToWrite;
        }

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

        // Save the new index to the iNode or the indirect block
        if (isUsingIndirectBlock) {
            // We have to write the pointer to the indirect block
            indirectBlockPointer->block[indexWhereDataGoesInIndirectBLock] = newBlockDiskIndex;
        } else {
            // We write the pointer to the iNode
            fileINode->pointer[currentBlockIndex] = newBlockDiskIndex;
        }

        // If the block is already partially full, then we need to load the old data
        if (!blockIsNew) {
            printf("Block is not new.\n");
            read_data_block(newBlockDiskIndex, newBuffer);
        }

        // Copy the desired amount of data onto it
        memcpy(newBuffer + startingIndexOfBlock, buf, (size_t) amountToWrite);

        printf("File %d writing %d bytes to disk block %d\n", fileID, amountToWrite, newBlockDiskIndex);

        // Write the new buffer to the hard drive
//        if (newBlockDiskIndex >= (DISK_BLOCK_COUNT - DATA_BLOCK_TABLE_INDEX)) {
//            printf("Disk is full!\n");
//            // Disk is full
//            return -1;
//            //break;
//        }

        write_data_block(newBlockDiskIndex, newBuffer);

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

    if (indirectBlockPointer) {
        // Write the data block to disk
        write_data_block(fileINode->ind_pointer, indirectBlockPointer);
        free(indirectBlockPointer);
    }

    // Free the new buffer
    free(newBuffer);

    // Save our cached version of the file system
    save_local_file_system_to_disk(freeBitMap, iNodeTable, directoryCache);

    printf("Finished writing iNode:\n");
    INode_print(*fileINode);

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
    int directoryIndex = -1;
    int i;
    for (i = 0; i < 10; i++) {
        if (strcmp(directoryCache->directory[i].name, file) == 0) {
            // We found it
            directoryIndex = i;
            break;
        }
    }

    if (directoryIndex == -1) {
        // Can't delete a non-existing file
        return -1;
    }

    int iNodeIndex = directoryCache->directory[directoryIndex].i_node_index;

    INode iNode = iNodeTable->i_node[iNodeIndex];
    for (i = 0; i < BLOCKS_PER_I_NODE; i++) {
        if (iNode.pointer[i] > -1) {
            // Erase the block from the disk
            erase_data_block(iNode.pointer[i]);
            // Mark block free
            FreeBitMap_markBitFree(freeBitMap, iNode.pointer[i]);
        }
    }

    if (iNode.ind_pointer > -1) {
        // Delete the blocks from the indirect pointer
        IndirectBlockPointer* indirectPointer = malloc(DISK_BLOCK_SIZE);
        read_data_block(iNode.ind_pointer, indirectPointer);

        // Delete the indirectly pointed blocks
        for (i = 0; i < INDIRECT_BLOCK_POINTER_SIZE; i++) {
            if (indirectPointer->block[i] > 0) {
                // Erase the block from the disk
                erase_data_block(indirectPointer->block[i]);
                // Mark block free
                FreeBitMap_markBitFree(freeBitMap, indirectPointer->block[i]);
            }
        }

        free(indirectPointer);
    }


    // Delete the iNode
    INodeTable_deleteINode(iNodeTable, iNodeIndex);

    // Delete the directory object
    DirectoryCache_deleteIndex(directoryCache, directoryIndex);

    // Save our cached version of the file system
    save_local_file_system_to_disk(freeBitMap, iNodeTable, directoryCache);

    printf("Finished removing %s.\n", file);

	return 0;
}
