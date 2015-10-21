//
// Created by Andrew on 2015-10-21.
//

#ifndef ASSIGNMENT_3_SFS_API_H
#define ASSIGNMENT_3_SFS_API_H

/**
 * creates the file system
 */
void mksfs(int fresh) {
    // TODO
}


/**
 * Get the name of the next file in directory
 */
int sfs_getnextfilename(char *fname) {
    // TODO
    return -1;
}


/**
 * Get the size of the given file
 */
int sfs_getfilesize(const char* path) {
    // TODO
    return -1;
}


/**
 * opens the given file
 */
int sfs_fopen(char *name) {
    // TODO
    return -1;
}


/**
 * Closes the given file
 */
void sfs_fclose(int fileID) {
    // TODO
}


/**
 * Write buf characters into disk
 */
void sfs_fwrite(int fileID, char *buf, int length) {
    // TODO
}


/**
 * read characters from disk into buf
 */
void sfs_fread(int fileID, char *buf, int length) {
    // TODO
}


/**
 * seek to the location from beginning
 */
void sfs_fseek(int fileID, int loc) {
    // TODO
}


/**
 * removes a file from the filesystem
 */
int sfs_remove(char *file) {
    // TODO
    return -1;
}


#endif //ASSIGNMENT_3_SFS_API_H
