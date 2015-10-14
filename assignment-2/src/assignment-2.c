/*
 ============================================================================
 Name        : assignment-2.c
 Author      : Andrew Fogarty - 260535895
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdlib.h>
#include "authentication.h"
#include "fifo_buffer.h"
#include "fifo_buffer_ops.h"
#include "print_job.h"
#include "shared_mem.h"

// Necessary for shared mem
#include <sys/ipc.h>
#include <sys/shm.h>

SharedMemory* shared_mem;

void setup_shared_mem() {
	shared_mem = malloc(sizeof(SharedMemory));
}

void attach_shared_mem() {

}

void init_semaphore() {

}

void take_a_job(PrintJob* job) {

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
