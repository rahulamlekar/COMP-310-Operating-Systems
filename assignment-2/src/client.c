/*
 ============================================================================
 Name        : client.c
 Author      : Andrew Fogarty - 260535895
 Version     :
 Copyright   :
 Description : Printer Client
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>

// Authentication
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

/**
 * Attach to the shared memory created by the server.
 */
SharedMemory* attach_share_mem(int printerId) {
	// Get the shmid of the desired shared memory
	int shmid = shmget(SHARED_MEM_KEY + printerId, sizeof(SharedMemory), 0);
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

/**
 * Add a job to the shared memory.
 */
void put_a_job(SharedMemory* memory, PrintJob job) {
    int neededToSleep;
    int complete = 0;

//    while (!complete) {
//        sem_wait(&memory->mutex);
//
//        // Wait or lock the semaphore
//        if(sem_trywait(&memory->empty) == 0) {
//            // We got through, continue
//            printf("Client %d has %d pages to print, puts request in Buffer[%d]\n", job.id, job.pagesToPrint, memory->buffer.tailIndex);
//
//            // Add the job to the buffer.
//            pushFifoBuffer(&memory->buffer, job);
//
//            // CRITICAL SECTION END
//
//            sem_post(&memory->full);
//            complete = 1;
//        } else {
//    //        // Let the user know that we are waiting
//            printf("Client %d has %d pages to print, buffer full, sleeps\n", job.id, job.pagesToPrint);
//    //        // Wait
//    //        //sem_wait(&memory->empty);
//        }
//        sem_post(&memory->mutex);
//
//    }

    int didWait = 0;

    if (willSemaphoreWait(&memory->empty)) {
        printf("Client %d has %d pages to print, buffer full, sleeps\n", job.id, job.pagesToPrint);
        didWait = 1;
    }

    sem_wait(&memory->empty);
    sem_wait(&memory->mutex);
    // Critical section

    // Let the user know what has happened
    if (didWait) {
        printf("Client %d wakes up, puts request in Buffer[%d]\n", job.id, memory->buffer.tailIndex);
    } else {
        printf("Client %d has %d pages to print, puts request in Buffer[%d]\n", job.id, job.pagesToPrint, memory->buffer.tailIndex);
    }

    // Add the job to the buffer.
    pushFifoBuffer(&memory->buffer, job);

    // End critical section
    sem_post(&memory->mutex);
    sem_post(&memory->full);
}

/**
 * Release the shared memory
 */
void release_share_mem(SharedMemory* sharedMemory) {
	shmdt(sharedMemory);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Please provide clientId, printerId, numPages, and duration arguments!\n");
        return EXIT_FAILURE;
    }

    // Get the client params
    int clientId = atoi(argv[1]);
    int printerId = atoi(argv[2]);
    int numPages = atoi(argv[3]);
    int duration = atoi(argv[4]);

	SharedMemory* sharedMemory = attach_share_mem(printerId);              // use the same key as the server so that the client can connect to the same memory segment

	PrintJob job = create_job(clientId, numPages, duration);  // create the job record
	put_a_job(sharedMemory, job);             // put the job record into the shared buffer
	release_share_mem(sharedMemory);        // release the shared memory
	return EXIT_SUCCESS;
}


