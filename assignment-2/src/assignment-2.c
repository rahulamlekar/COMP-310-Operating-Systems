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
    // TODO: Nuke pre-existing shared memory
	// Create some shared memory
    // O77 sets the permissions
	shmid = shmget(SHARED_MEM_KEY, sizeof(SharedMemory), 0777 | IPC_CREAT);
}

/**
 * Attach the shared memory segment
 */
void attach_shared_mem() {
	// Attach to the shared memory
	sharedMemory = (void *) shmat(shmid, NULL, 0);

    FifoBuffer* buffer = &sharedMemory->buffer;

    // Reset the values
    buffer->headIndex = 0;
    buffer->tailIndex = 0;

    int i;
    for (i = 0; i < BUFFER_SIZE; i++) {
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
void init_semaphore() {
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
    int neededToSleep;
    // Wait or lock the semaphore
    if (sem_trywait(&sharedMemory->full) == 0) {
        // It worked, we will continue
        neededToSleep = 0;
    } else {
        // The buffer is empty, so we will print a message and then wait
        printf("No request in buffer, Printer 0 sleeps\n");
        neededToSleep = 1;
        // Actually wait
        sem_wait(&sharedMemory->full);
    }
    sem_wait(&sharedMemory->mutex);
    // CRITICAL SECTION BEGIN

    // Pop the job off the buffer
    PrintJob sharedMemJob = popFifoBuffer(&sharedMemory->buffer);

    *job = sharedMemJob;

    // CRITICAL SECTION END
    sem_post(&sharedMemory->mutex);
    sem_post(&sharedMemory->empty);
}

/**
 * Sleep for the duration of a job.
 */
void go_sleep(PrintJob* job) {
    // Let the user know that we are starting to sleep
    printf("Printer 0 starts printing %d pages from Buffer[%d]\n", job->pagesToPrint, sharedMemory->buffer.lastPoppedIndex);
    // If we don't flush the buffer, then the print doesn't happen before the sleep
    fflush(stdout);
    sleep(job->duration);
    printf("Printer 0 finishes printing %d pages from Buffer[%d]\n", job->pagesToPrint, sharedMemory->buffer.lastPoppedIndex);

}

int main(int argc, char *argv[]) {
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
        go_sleep(&job);    // sleep for job duration
	}

	return EXIT_SUCCESS;
}
