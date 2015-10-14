/*
 * client.c
 *
 *  Created on: Oct 11, 2015
 *      Author: Andrew Fogarty
 */

#include <stdlib.h>
#include "authentication.h"
#include "fifo_buffer.h"
#include "fifo_buffer_ops.h"
#include "print_job.h"

FifoBuffer* buffer;

void attach_share_mem() {
	buffer = malloc(sizeof(FifoBuffer));
}

void place_params() {

}

void get_job_params() {

}

PrintJob create_job() {
	return (PrintJob) {
		0,
		0
	};
}

void put_a_job(PrintJob* job) {

}

/**
 * Release the shared memory
 */
void release_share_mem() {

}

int main() {
	attach_share_mem();              // use the same key as the server so that the client can connect to the same memory segment
	place_params();                 // this is to place the parameters in the shared memory � in particular the job queue and semaphore
	get_job_params();              // read the terminal and get the job params
	PrintJob job = create_job();  // create the job record
	put_a_job(&job);             // put the job record into the shared buffer
	release_share_mem();        // release the shared memory

	return 0;
}


