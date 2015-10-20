/*
 * fifo_buffer.h
 *
 *  Created on: Oct 13, 2015
 *      Author: Andrew
 */

#include "print_job.h"

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

#ifndef FIFO_BUFFER_H_
#define FIFO_BUFFER_H_

typedef struct fifo_buffer {
	PrintJob elements[MAX_BUFFER_SIZE];
	int headIndex;
	int tailIndex;
	int size;
} FifoBuffer;

#endif /* FIFO_BUFFER_H_ */
