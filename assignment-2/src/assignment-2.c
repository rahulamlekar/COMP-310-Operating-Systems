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

/*
 * Helper functions
 */
void print_error() {
    //printf("Error: %d\n", errno);
}


/**
 * Create a shared memory segment
 */
void setup_shared_mem() {
    //printf("Size: %d.\n", sizeof(SharedMemory));
    //printf("Min: %d.   Max: %d.\n", SHMMIN, SHMMAX);
	// Create some shared memory
    // O77 sets the permissions
	shmid = shmget(SHARED_MEM_KEY, sizeof(SharedMemory), 0777 | IPC_CREAT);
	//printf("Shmid: %d\n", shmid);
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

/**
 * Initialize the semaphore and put it in shared memory.
 */
void init_semaphore() {
    sem_init(
            &sharedMemory->mutex,
            1, // Inter-process
            1 // Start at 1
    );
	sem_init(
            &sharedMemory->empty,
            1,
            1 - BUFFER_SIZE
    );
    sem_init(
            &sharedMemory->full,
            1,
            0
    );
    //print_error();
}

void take_a_job(PrintJob* job, int* bufferIsEmptyFlag) {
    printf("Begin taking job.\n");
    // Wait or lock the semaphore
    sem_wait(&sharedMemory->full);
    printf("Server past full.\n");
    sem_wait(&sharedMemory->mutex);
    // CRITICAL SECTION BEGIN
    printf("Server is in critical section.\n");
        //printf("Latest Test: %p\n", popFifoBuffer(&sharedMemory->buffer));
        // Copy the next print job off of the shared buffer
    // Get the job object in shared memory
    PrintJob* sharedMemJob = popFifoBuffer(&sharedMemory->buffer);
    printf("Latest Test: %p\n", sharedMemJob);


    //copyPrintJob(sharedMemJob, job);

    printf("Server is leaving critical section.\n");
    // CRITICAL SECTION END
    sem_post(&sharedMemory->mutex);
    sem_post(&sharedMemory->empty);
    //printf("End taking job.\n");
}

/**
 * Print the properties of a job.
 */
void print_a_msg(PrintJob* job) {
    printPrintJob(*job);
}

/**
 * Sleep for the duration of a job.
 */
void go_sleep(PrintJob* job) {
    printf("Printer 0 starts printing 8 pages from Buffer[1]");
	//printf("Starting to sleep.\n");
	sleep(job->duration);
	//printf("Finished sleeping.\n");
}

int main(void) {
	setup_shared_mem();    // create a shared memory segment
	attach_shared_mem();   // attach the shared memory segment
	init_semaphore();      // initialize the semaphore and put it in shared memory

    // An empty job schema that we will fill up to print
    PrintJob job = {
            0, 0, 0
    };

    int bufferIsEmptyFlag = 0;

	while (1) {
		take_a_job(&job, &bufferIsEmptyFlag);  // this is blocking on a semaphore if no job
        print_a_msg(&job); // duration of job, job ID, source of job are printed
        go_sleep(&job);    // sleep for job duration
	}

	return EXIT_SUCCESS;
}
