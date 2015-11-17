#include <stdlib.h>
#include <stdio.h>
#include "sfs_api.h"
#include "constants.h"
#include "data_structures/super_block.h"
#include "data_structures/file_descriptor_table.h"
#include "disk_emu.h"
#include "data_structures/free_bitmap.h"
#include "data_structures/directory_cache.h"
#include "data_structures/indirect_block_pointer.h"
#include "data_structures/i_node_table.h"
#include "helpers/disk_access.h"

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

    // Make sure the iNodeTable is reset
    INodeTable_reset(iNodeTable);
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
    int j;
    for (j = 0; j < I_NODE_COUNT; j++) {
        printf("iNode %d size: %d\n", j, iNodeTable->i_node[j].size);
    }
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

    int directoryIndex = DirectoryCache_getNameIndex(directoryCache, name);
    if (directoryIndex == -1) {
        // The directory doesn't exist, so we will create it
        directoryIndex = DirectoryCache_getOpenIndex(*directoryCache);
        if (directoryIndex == -1) {
            printf("Directory index full!\n");
            // No more directories
            return -1;
        }
        DirectoryCache_markClosed(directoryCache, directoryIndex);

        // Copy the name
        strcpy(directoryCache->directory[directoryIndex].name, name);
        directoryCache->directory[directoryIndex].i_node_index = -1;
    }

    printf("opening directory: %d\n", directoryIndex);

    // Configure the iNode if Necessary
    int iNodeIndex = directoryCache->directory[directoryIndex].i_node_index;
    if (iNodeIndex == -1) {
        iNodeIndex = INodeTable_getOpenIndex(*iNodeTable);
        if (iNodeIndex == -1) {
            printf("iNode index full!\n");
            // No more iNodes
            return -1;
        }
        INodeTable_markClosed(iNodeTable, iNodeIndex);

        INode_new(&iNodeTable->i_node[iNodeIndex]);
        directoryCache->directory[directoryIndex].i_node_index = iNodeIndex;
    }
    FileDescriptorTable_print(*fileDescriptorTable);
    printf("opening iNode: %d\n", iNodeIndex);

    // Configure the FD table
    int fdIndex = FileDescriptorTable_getIndexOfInode(*fileDescriptorTable, iNodeIndex);
    printf("test fdIndex: %d\n", fdIndex);
    if (fdIndex == -1) {
        // Get a new fd table index
        fdIndex = FileDescriptorTable_getOpenIndex(*fileDescriptorTable);
        if (fdIndex == -1) {
            printf("FD index full!\n");
            // No more file descriptors
            return -1;
        }
        FileDescriptorTable_markInUse(fileDescriptorTable, fdIndex);

        // Point to the iNode
        fileDescriptorTable->fd[fdIndex].i_node_number = iNodeIndex;
        // Open in append mode
        fileDescriptorTable->fd[fdIndex].read_write_pointer = iNodeTable->i_node[iNodeIndex].size;
    } else {
        // If we don't do this, then the 0 index gets messed up!
        FileDescriptorTable_markInUse(fileDescriptorTable, fdIndex);
    }

    printf("File %d opened.\n", fdIndex);

    DirectoryCache_print(*directoryCache);
    FileDescriptorTable_print(*fileDescriptorTable);

    return fdIndex;
}


/**
 * closes the given file
 */
int sfs_fclose(int fileID) {
    if (fileID < 0) {
        printf("Invalid FileID!\n");
        return -1;
    }

    fprintf(stderr, "Closing file %d\n", fileID);

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

    printf("Post close:\n");

    FileDescriptorTable_print(*fileDescriptorTable);

    // Success!
	return 0;
}


/**
 * read characters from disk into buf
 */
int sfs_fread(int fileID, char *buf, int length){
//    if (fileID < 0) {
//        printf("Invalid FileID!\n");
//        return -1;
//    }
    printf("Reading %d bytes from file %d\n", length, fileID);

//    // Return if file is closed
    if (FileDescriptorTable_isNotInUse(*fileDescriptorTable, fileID)) {
        printf("File %d is closed.  Cannot read...\n", fileID);
        return 0;
    }

    FileDescriptor* fd = &fileDescriptorTable->fd[fileID];
    INode iNode = iNodeTable->i_node[fd->i_node_number];

    //("Reading iNode:\n");
    INode_print(iNode);

    // Output is the total data that we'have read...
    int output = 0;

    int amountLeftToRead;
    if ((fd->read_write_pointer + length + 1) > iNode.size) {
        printf("size: %d, rwpointer: %d\n", iNode.size, fd->read_write_pointer);
        // Read what's left of the file
        amountLeftToRead = iNode.size - fd->read_write_pointer;
    } else {
//        // Read the whole file
        amountLeftToRead = length;
    }

    //printf("AmountLeftToRead: %d\n", amountLeftToRead);

    // We mighth have to access the indirect block
    IndirectBlockPointer* indirectBlockPointer = malloc(DISK_BLOCK_SIZE);
    if (iNode.ind_pointer > -1) {
        read_data_block(iNode.ind_pointer, indirectBlockPointer);
        printf("Loaded indirect block: %d\n", iNode.ind_pointer);
    }

    //printf("rw pointer: %d\n", fd->read_write_pointer);

    // Allocate an empty block
    void* data = malloc(DISK_BLOCK_SIZE);

    while (amountLeftToRead > 0) {
        printf("Current RW pointer %d\n", fd->read_write_pointer);

        int currentBlockIndex = fd->read_write_pointer / DISK_BLOCK_SIZE;
        int startingIndexOfBlock = fd->read_write_pointer % DISK_BLOCK_SIZE;

        int amountToRead;
        if (amountLeftToRead > (DISK_BLOCK_SIZE - startingIndexOfBlock)) {
            amountToRead = DISK_BLOCK_SIZE - startingIndexOfBlock;
        } else {
            amountToRead = amountLeftToRead;
        }

        // Empty the data buffer
        memset(data, '\0', DISK_BLOCK_SIZE);

        // Read data from the correct source
        printf("IsIndexInIndirectBlock(%d) = %d, indirectBlockIndex(%d) = %d\n",
               currentBlockIndex,
               isIndexInIndirectBlock(currentBlockIndex),
               currentBlockIndex,
               indirectBlockIndex(currentBlockIndex));
        IndirectBlockPointer_print(*indirectBlockPointer);
        if (isIndexInIndirectBlock(currentBlockIndex)) {
            printf("File %d reading %d bytes indirectly from disk block %d\n", fileID, amountToRead, indirectBlockPointer->block[indirectBlockIndex(currentBlockIndex)]);
            read_data_block(indirectBlockPointer->block[indirectBlockIndex(currentBlockIndex)], data);
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

    free(indirectBlockPointer);

    printf("Finished reading %d of %d requested bytes.\n", output, length);

	return output;
}


/**
 * write buf characters into disk
 */
int sfs_fwrite(int fileID, const char *buf, int length){
//    if (fileID < 0) {
//        printf("Invalid FileID!\n");
//        return -1;
//    }
    printf("Start write procedure to write %d bytes to file %d\n", length, fileID);

    // Get the iNode of the desired file
    FileDescriptor* fileDescriptor = &fileDescriptorTable->fd[fileID];
    INode* fileINode = &iNodeTable->i_node[fileDescriptor->i_node_number];

    // Keep track of how much we have written, and how much we must write
    int totalBytesWritten = 0;
    int totalAmountLeftToWrite = length;

    // Create an empty buffer
    void* newBuffer = malloc(DISK_BLOCK_SIZE);

    /*
     * Set up the indirect block
     */

    // We might have to access the indirect block
    IndirectBlockPointer* indirectBlock = malloc(DISK_BLOCK_SIZE);
    // Initialize indirect buffer if necessary
    if (fileINode->ind_pointer < 0) {
        // Create an index for the ind pointer
        fileINode->ind_pointer = FreeBitMap_getFreeBitAndMarkUnfree(freeBitMap);
        if (fileINode->ind_pointer == -1) {
            // No space left!
            return -1;
        }
    } else {
        // There already is an ind pointer, so we load from disk
        read_data_block(fileINode->ind_pointer, indirectBlock);
    }

    while (totalAmountLeftToWrite > 0) {
        // Empty the contents of newBuffer
        memset(newBuffer, '\0', DISK_BLOCK_SIZE);

        printf("Current rw pointer: %d\n", fileDescriptor->read_write_pointer);

        int currentBlockIndex = fileDescriptor->read_write_pointer / DISK_BLOCK_SIZE;
        int startingIndexOfBlock = fileDescriptor->read_write_pointer % DISK_BLOCK_SIZE;

        printf("currentBlockIndex: %d, startingIndexOfBlock: %d\n", currentBlockIndex, startingIndexOfBlock);

        int amountToWrite;
        if (totalAmountLeftToWrite > (DISK_BLOCK_SIZE - startingIndexOfBlock)) {
            amountToWrite = DISK_BLOCK_SIZE - startingIndexOfBlock;
        } else {
            amountToWrite = totalAmountLeftToWrite;
        }

        /*
         * Handle getting the disk index for the data
         */

        int blockIsNew = 0;
        int diskIndex;
        if (isIndexInIndirectBlock(currentBlockIndex)) {
            // We are using the indirect block
            diskIndex = indirectBlock->block[indirectBlockIndex(currentBlockIndex)];
            if (diskIndex <= 0) {
                blockIsNew = 1;
                // Get a new bitmap index
                diskIndex = FreeBitMap_getFreeBitAndMarkUnfree(freeBitMap);
                if (diskIndex == -1) {
                    // Disk is full!
                    break;
                }
                // Save it to the indirect block
                indirectBlock->block[indirectBlockIndex(currentBlockIndex)] = diskIndex;
            }
        } else {
            // We are using the iNode Directly
            diskIndex = fileINode->pointer[currentBlockIndex];
            if (diskIndex < 0) {
                blockIsNew = 1;
                // Get a new bitmap index
                diskIndex = FreeBitMap_getFreeBitAndMarkUnfree(freeBitMap);
                if (diskIndex == -1) {
                    // Disk is full!
                    break;
                }
                fileINode->pointer[currentBlockIndex] = diskIndex;
            }
        }

        // If the block is already partially full, then we need to load the old data
        if (!blockIsNew) {
            //printf("Block is not new.\n");
            read_data_block(diskIndex, newBuffer);
        }

        // Copy the desired amount of data onto it
        memcpy(newBuffer + startingIndexOfBlock, buf, (size_t) amountToWrite);

        //printf("File %d writing %d bytes to disk block %d\n", fileID, amountToWrite, newBlockDiskIndex);

        // Write the new buffer to the hard drive
//        if (newBlockDiskIndex >= (DISK_BLOCK_COUNT - DATA_BLOCK_TABLE_INDEX)) {
//            printf("Disk is full!\n");
//            // Disk is full
//            return -1;
//            //break;
//        }

        write_data_block(diskIndex, newBuffer);

        // Save how much we have written to the file pointer
        fileDescriptor->read_write_pointer += amountToWrite;

        // Track how much we have written.  This will be returned to user
        totalBytesWritten += amountToWrite;
        totalAmountLeftToWrite -= amountToWrite;

        // Increment the buffer
        buf += amountToWrite;

        // Record file size change
        if (fileDescriptor->read_write_pointer > fileINode->size) {
            fileINode->size = fileDescriptor->read_write_pointer;
        }
    }

    // Write the data block to disk
    write_data_block(fileINode->ind_pointer, indirectBlock);

    IndirectBlockPointer_print(*indirectBlock);
    free(indirectBlock);

    // Free the new buffer
    free(newBuffer);

    // Save our cached version of the file system
    save_local_file_system_to_disk(freeBitMap, iNodeTable, directoryCache);

    INode_print(*fileINode);

    //fprintf(stderr, "Wanted to write %d bytes, actually wrote %d\n", length, totalBytesWritten);

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
    int directoryIndex = DirectoryCache_getNameIndex(directoryCache, file);

    if (directoryIndex == -1) {
        // Can't delete a non-existing file
        return -1;
    }

    int iNodeIndex = directoryCache->directory[directoryIndex].i_node_index;

    INode iNode = iNodeTable->i_node[iNodeIndex];
    int i;
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
