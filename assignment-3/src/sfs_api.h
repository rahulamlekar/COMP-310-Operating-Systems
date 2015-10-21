//
// Created by Andrew on 2015-10-21.
//

#ifndef ASSIGNMENT_3_SFS_API_H
#define ASSIGNMENT_3_SFS_API_H

extern void mksfs(int fresh); // creates the file system
extern int sfs_getnextfilename(char *fname); // get the name of the next file in directory
extern int sfs_getfilesize(const char* path); // get the size of the given file
extern int sfs_fopen(char *name); // opens the given file
extern void sfs_fclose(int fileID); // closes the given file
extern void sfs_fwrite(int fileID, char *buf, int length); // write buf characters into disk
extern void sfs_fread(int fileID, char *buf, int length); // read characters from disk into buf
extern void sfs_fseek(int fileID, int loc); // seek to the location from beginning
extern int sfs_remove(char *file); // removes a file from the filesystem

#endif //ASSIGNMENT_3_SFS_API_H
