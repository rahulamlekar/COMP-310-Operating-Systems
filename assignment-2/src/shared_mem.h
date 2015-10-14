/*
 * shared_mem.h
 *
 *  Created on: Oct 13, 2015
 *      Author: Andrew
 */

#ifndef SHARED_MEM_H_
#define SHARED_MEM_H_

#include "fifo_buffer.h"

typedef struct shared_mem {
	FifoBuffer buffer;
} SharedMemory;

#endif /* SHARED_MEM_H_ */
