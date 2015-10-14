/*
 * fifo_buffer.h
 *
 *      Author: Andrew Fogarty - 260535895
 */

#include "print_job.h"
#include "fifo_buffer.h"

#ifndef FIFO_BUFFER_OPS_H_
#define FIFO_BUFFER_OPS_H_

extern int pushFifoBuffer(FifoBuffer* buffer, PrintJob* newValue);
extern PrintJob* popFifoBuffer(FifoBuffer* buffer);

#endif /* FIFO_BUFFER_OPS_H_ */
