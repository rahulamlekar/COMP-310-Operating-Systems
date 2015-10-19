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
	// Attach to the shared memory
    SharedMemory* output = NULL;
	output = shmat(shmid, output, 0);

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
    int neededToSleep;
    int complete = 0;

    while (!complete) {
        sem_wait(&memory->mutex);

        // Wait or lock the semaphore
        if(sem_trywait(&memory->empty) == 0) {
            // We got through, continue
            printf("Client %d has %d pages to print, puts request in Buffer[%d]\n", job.id, job.pagesToPrint, memory->buffer.tailIndex);

            // Add the job to the buffer.
            pushFifoBuffer(&memory->buffer, job);

            // CRITICAL SECTION END

            sem_post(&memory->full);
            complete = 1;
        } else {
    //        // Let the user know that we are waiting
    //        printf("Client %d has %d pages to print, buffer full, sleeps\n", job.id, job.pagesToPrint);
    //        // Wait
    //        //sem_wait(&memory->empty);
        }
        sem_post(&memory->mutex);

    }
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
	return EXIT_SUCCESS;
}


