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

SharedMemory* attach_share_mem() {
	// Get the shmid of the desired shared memory
	int shmid = shmget(SHARED_MEM_KEY, sizeof(SharedMemory), 0);
    printf("Shmid: %d\n", shmid);
	// Attach to the shared memory
    SharedMemory* output = NULL;
	output = shmat(shmid, output, 0);
    printf("SharedMemory: %p\n", output);

    return output;
}

/**
 * Create the job record
 */
PrintJob create_job(int id, int numPages, int duration) {
    // The job we will output
    return (PrintJob) {
            id,
            duration,
            numPages
    };
}

void put_a_job(SharedMemory* memory, PrintJob job) {
    printf("Begin put_a_job\n");
    // Wait or lock the semaphore
    sem_wait(&memory->empty);
    printf("Past empty semaphore\n");
    sem_wait(&memory->mutex);
    printf("Past mutex semaphore.\n");
    // CRITICAL SECTION BEGIN
    printf("Client inside critical section.\n");

    // Add the job to the buffer.
    pushFifoBuffer(&memory->buffer, job);

    // CRITICAL SECTION END
    sem_post(&memory->mutex);
    sem_post(&memory->full);
    printf("Client outside critical section.\n");
}

/**
 * Release the shared memory
 */
void release_share_mem(SharedMemory* sharedMemory) {
	shmdt(sharedMemory);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Please provide clientId, numPages, and duration arguments!\n");
        return EXIT_FAILURE;
    }

    // Get the client params
    int clientId = atoi(argv[1]);
    int numPages = atoi(argv[2]);
    int duration = atoi(argv[3]);

	SharedMemory* sharedMemory = attach_share_mem();              // use the same key as the server so that the client can connect to the same memory segment

	PrintJob job = create_job(clientId, numPages, duration);  // create the job record
	put_a_job(sharedMemory, job);             // put the job record into the shared buffer
	release_share_mem(sharedMemory);        // release the shared memory
    printf("Client terminated.\n");

	return EXIT_SUCCESS;
}


