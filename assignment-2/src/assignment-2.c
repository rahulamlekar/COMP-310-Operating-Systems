/*
 ============================================================================
 Name        : assignment-2.c
 Author      : Andrew Fogarty - 260535895
 Version     :
 Copyright   : 
 Description : Printer Spooler server
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

// Sleep
#include <unistd.h>

// Custom code written by me
#include "authentication.h"
#include "fifo_buffer.h"
#include "fifo_buffer_ops.h"
#include "print_job.h"
#include "shared_mem.h"

// Necessary for shared mem
//#define _SVID_SOURCE
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

// POSIX semaphores
#include <semaphore.h>
#include <errno.h>


/**
 * The id that we will use to access shared memory.
 */
int shmid;
/**
 * Pointer to the memory that is shared
 */
SharedMemory* sharedMemory;

/**
 * Create a shared memory segment
 */
void setup_shared_mem() {
    printf("Size: %d.\n", sizeof(SharedMemory));
    printf("Min: %d.   Max: %d.\n", SHMMIN, SHMMAX);
	// Create some shared memory
    // O77 sets the permissions
	shmid = shmget(SHARED_MEM_KEY, sizeof(SharedMemory), 0777 | IPC_CREAT);
	printf("Shmid: %d\n", shmid);
    print_error();
}

/**
 * Attach the shared memory segment
 */
void attach_shared_mem() {
	// Attach to the shared memory
	sharedMemory = (void *) shmat(shmid, NULL, 0);
	printf("Test: %d\n", (int) sharedMemory);
    //printf("Error: %d; %d; %d; %d; %d\n", errno, EACCES, EIDRM, EINVAL, ENOMEM);
}

void take_a_job(PrintJob* job) {

}

/**
 * Initialize the semaphore and put it in shared memory.
 */
void init_semaphore() {
	//sharedMemory->semaphore = NULL;
    //sharedMemory->buffer.headIndex = 7;

	//sem_init(sharedMemory->semaphore, 1, 0);
    //print_error();
}

void print_error() {
    printf("Error: %d\n", errno);
}



/**
 * Print the properties of a job.
 */
void print_a_msg(PrintJob* job) {

}

/**
 * Sleep for the duration of a job.
 */
void go_sleep(PrintJob* job) {
	//printf("Starting to sleep.\n");
	sleep(job->duration);
	//printf("Finished sleeping.\n");
}

int main(void) {
	setup_shared_mem();    // create a shared memory segment
	attach_shared_mem();   // attach the shared memory segment
	init_semaphore();      // initialize the semaphore and put it in shared memory

	PrintJob job = {
			0,
			0
	};

	//while (1) {
		take_a_job(&job);  // this is blocking on a semaphore if no job
		print_a_msg(&job); // duration of job, job ID, source of job are printed
		go_sleep(&job);    // sleep for job duration
	//}

	return EXIT_SUCCESS;
}
