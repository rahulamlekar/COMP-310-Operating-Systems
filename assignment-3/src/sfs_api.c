#include "sfs_api.h"


/**
 * creates the file system
 */
void mksfs(int fresh) {
	//Implement mksfs here	
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

	//Implement sfs_getfilesize here	
	return 0;
}


/**
 * opens the given file
 */
int sfs_fopen(char *name) {

	//Implement sfs_fopen here	
	return 0;
}


/**
 * closes the given file
 */
int sfs_fclose(int fileID){

	//Implement sfs_fclose here	
	return 0;
}


/**
 * read characters from disk into buf
 */
int sfs_fread(int fileID, char *buf, int length){

	//Implement sfs_fread here	
	return 0;
}


/**
 * write buf characters into disk
 */
int sfs_fwrite(int fileID, const char *buf, int length){

	//Implement sfs_fwrite here	
	return 0;
}


/**
 * seek to the location from beginning
 */
int sfs_fseek(int fileID, int loc){

	//Implement sfs_fseek here	
	return 0;
}


/**
 * removes a file from the filesystem
 */
int sfs_remove(char *file) {

	//Implement sfs_remove here	
	return 0;
}
