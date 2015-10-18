/*
 * client.c
 *
 *  Created on: Oct 11, 2015
 *      Author: Andrew Fogarty
 */

#include <stdio.h>
#include <stdlib.h>
#include "authentication.h"
#include "fifo_buffer.h"
#include "fifo_buffer_ops.h"
#include "print_job.h"
#include "shared_mem.h"

// Necessary for shared mem
#include <sys/ipc.h>
#include <sys/shm.h>

// POSIX semaphores
#include <semaphore.h>

SharedMemory* sharedMemory;

// Global vars
unsigned int currentJobId = 0;
unsigned int duration = 0;
unsigned int pagesToPrint = 0;

void attach_share_mem() {
	// Get the shmid of the desired shared memory
	int shmid = shmget(SHARED_MEM_KEY, sizeof(SharedMemory), 0);
    printf("Shmid: %d\n", shmid);
	// Attach to the shared memory
	sharedMemory = shmat(shmid, sharedMemory, 0);
    printf("SharedMemory: %p\n", sharedMemory);
}


/**
 * This is to place the parameters in the shared memory – in particular the job queue and semaphore.
 */
void place_params() {

}


/**
 * Read the terminal and get the job params
 */
PrintJob get_job_params() {
    printf("Please input parameters for new print job.\n");
    // Pages to print
    printf("Number of pages: ");
    scanf("%d", &pagesToPrint);
    // Pages to print
    printf("Job duration (seconds): ");
    scanf("%d", &duration);
}


/**
 * Create the job record
 */
PrintJob create_job() {
    // The job we will output
    PrintJob output = {
            currentJobId,
            duration,
            pagesToPrint
    };

    // Increment the jobId for next time
    currentJobId++;

    return output;
}

void put_a_job(PrintJob* job) {
    printf("Begin put_a_job\n");
    printf("Semaphore: %p.\n", &sharedMemory->semaphore);
    // Wait or lock the semaphore
    sem_wait(&sharedMemory->semaphore);
    // CRITICAL SECTION BEGIN
    printf("Client inside critical section.\n");

    // Add the job to the buffer.
    pushFifoBuffer(&sharedMemory->buffer, job);

    // CRITICAL SECTION END
    sem_post(&sharedMemory->semaphore);
    printf("Client outside critical section.\n");
}

/**
 * Release the shared memory
 */
void release_share_mem() {
	shmdt(sharedMemory);
}

int main() {
	attach_share_mem();              // use the same key as the server so that the client can connect to the same memory segment
	place_params();                 // this is to place the parameters in the shared memory – in particular the job queue and semaphore
	get_job_params();              // read the terminal and get the job params
	PrintJob job = create_job();  // create the job record
	put_a_job(&job);             // put the job record into the shared buffer
	release_share_mem();        // release the shared memory
    printf("Client terminated.\n");
	return 0;
}


