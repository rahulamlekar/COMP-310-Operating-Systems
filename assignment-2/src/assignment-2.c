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
void setup_shared_mem(int printerId) {
	// Create some shared memory
    // O77 sets the permissions
	shmid = shmget(SHARED_MEM_KEY + printerId, sizeof(SharedMemory), 0777 | IPC_CREAT);
}

/**
 * Attach the shared memory segment
 */
void attach_shared_mem(int bufferSize) {
	// Attach to the shared memory
	sharedMemory = (void *) shmat(shmid, NULL, 0);

    FifoBuffer* buffer = &sharedMemory->buffer;

    // Reset the values
    buffer->headIndex = 0;
    buffer->tailIndex = 0;
    buffer->size = bufferSize;
    int i;
    for (i = 0; i < MAX_BUFFER_SIZE; i++) {
        PrintJob* current;
        current = &sharedMemory->buffer.elements[i];
        current->id = 0;
        current->duration = 0;
        current->pagesToPrint = 0;
    }
}

/**
 * Initialize the semaphore and put it in shared memory.
 */
void init_semaphore(int bufferSize) {
    // Destroy previously existing semaphores
    sem_destroy(&sharedMemory->mutex);
    sem_destroy(&sharedMemory->empty);
    sem_destroy(&sharedMemory->full);

    sem_init(
            &sharedMemory->mutex,
            1, // Inter-process
            1 // Start at 1
    );
	sem_init(
            &sharedMemory->empty,
            1,
            bufferSize
    );
    sem_init(
            &sharedMemory->full,
            1,
            0
    );
    //print_error();
}

int take_a_job(PrintJob* job, int* bufferIsEmptyFlag, int printerId) {

    // Let the user know if the printer is going to be sleeping
    if (willSemaphoreWait(&sharedMemory->full)) {
        printf("No request in buffer, Printer %d sleeps\n", printerId);
    }
    // Actually wait
    sem_wait(&sharedMemory->full);
    sem_wait(&sharedMemory->mutex);
    // CRITICAL SECTION BEGIN

    // Get the index that we will pop from
    int output = sharedMemory->buffer.headIndex;

    // Pop the job off the buffer
    PrintJob sharedMemJob = popFifoBuffer(&sharedMemory->buffer);

    *job = sharedMemJob;

    // CRITICAL SECTION END
    sem_post(&sharedMemory->mutex);
    sem_post(&sharedMemory->empty);

    // Return the index of the job from the buffer
    return output;
}

/**
 * Sleep for the duration of a job.
 */
void go_sleep(PrintJob* job, int bufferIndex, int printerId) {
    // Let the user know that we are starting to sleep
    printf("Printer %d starts printing %d pages from Buffer[%d]\n", printerId, job->pagesToPrint, bufferIndex);
    // If we don't flush the buffer, then the print doesn't happen before the sleep
    fflush(stdout);
    sleep(job->duration);
    printf("Printer %d finishes printing %d pages from Buffer[%d]\n", printerId, job->pagesToPrint, bufferIndex);

}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Please provide printerId and bufferSize arguments!\n");
        return EXIT_FAILURE;
    }

    // Get the printer params
    int printerId = atoi(argv[1]);
    int bufferSize = atoi(argv[2]);

    if (bufferSize < 0 || bufferSize < MAX_BUFFER_SIZE) {
        printf("Buffer size must be at least 0 and at most %d.\n", MAX_BUFFER_SIZE);
        return EXIT_FAILURE;
    }

	setup_shared_mem(printerId);    // create a shared memory segment
	attach_shared_mem(bufferSize);   // attach the shared memory segment
	init_semaphore(bufferSize);      // initialize the semaphore and put it in shared memory

    // An empty job schema that we will fill up to print
    PrintJob job = {
            0, 0, 0
    };

    int bufferIsEmptyFlag = 0;

	while (1) {
        int bufferIndex;
		bufferIndex = take_a_job(&job, &bufferIsEmptyFlag, printerId);  // this is blocking on a semaphore if no job
        go_sleep(&job, bufferIndex, printerId);    // sleep for job duration
	}

	return EXIT_SUCCESS;
}
