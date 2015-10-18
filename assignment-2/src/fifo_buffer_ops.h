/*
 * fifo_buffer.h
 *
 *      Author: Andrew Fogarty - 260535895
 */

#include "print_job.h"
#include "fifo_buffer.h"

#ifndef FIFO_BUFFER_OPS_H_
#define FIFO_BUFFER_OPS_H_

int isBufferEmpty(FifoBuffer* buffer) {
    return buffer->numberOfMembers <= 0;
}

int isBufferFull(FifoBuffer* buffer) {
    return buffer->numberOfMembers >= BUFFER_SIZE;
}

int pushFifoBuffer(FifoBuffer* buffer, PrintJob* newValue) {
    if (!buffer->elementFull[buffer->tailIndex]) {
        // We can proceed
        buffer->elementFull[buffer->tailIndex] = 1;
        buffer->elements[buffer->tailIndex] = newValue;
        buffer->tailIndex = (buffer->tailIndex + 1) % BUFFER_SIZE;
        // Increase the number of members
        buffer->numberOfMembers++;
        // It worked!
        return 1;
    }

    // It didn't work
    return 0;
}

PrintJob* popFifoBuffer(FifoBuffer* buffer) {
    // Grab the element we will take
    PrintJob* output = buffer->elements[buffer->headIndex];
    buffer->lastPoppedIndex = buffer->headIndex;
    // Mark it as empty
    buffer->elementFull[buffer->headIndex] = 0;
    // Move the index along
    buffer->headIndex = (buffer->headIndex + 1) % BUFFER_SIZE;
    // Decrease the number of members
    buffer->numberOfMembers--;
    // Return the value
    return output;
}

#endif /* FIFO_BUFFER_OPS_H_ */
