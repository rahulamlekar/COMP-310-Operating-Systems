/*
 * fifo_buffer.h
 *
 *  Created on: Oct 13, 2015
 *      Author: Andrew
 */

#include "print_job.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 6
#endif

#ifndef FIFO_BUFFER_H_
#define FIFO_BUFFER_H_

typedef struct fifo_buffer {
	PrintJob elements[BUFFER_SIZE];
	int elementFull[BUFFER_SIZE];
	int numberOfMembers;
	int headIndex;
	int tailIndex;
    int lastPoppedIndex;
} FifoBuffer;

#endif /* FIFO_BUFFER_H_ */
