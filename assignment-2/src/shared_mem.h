/*
 * shared_mem.h
 *
 *  Created on: Oct 13, 2015
 *      Author: Andrew
 */

#ifndef SHARED_MEM_H_
#define SHARED_MEM_H_

#include "fifo_buffer.h"
// POSIX semaphores
#include <semaphore.h>

typedef struct shared_mem {
	FifoBuffer buffer;
	sem_t mutex;
	sem_t empty;
	sem_t full;
} SharedMemory;

/**
 * Return 1 if semaphore will wait.  0 if not.
 */
int willSemaphoreWait(sem_t* semaphore) {
	int semaphoreValue;
	sem_getvalue(semaphore, &semaphoreValue);
	return semaphoreValue < 1;
}


#endif /* SHARED_MEM_H_ */
