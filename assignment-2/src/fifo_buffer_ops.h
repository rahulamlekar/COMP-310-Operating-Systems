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

void pushFifoBuffer(FifoBuffer* buffer, PrintJob newValue) {
    buffer->elementFull[buffer->tailIndex] = 1;
    buffer->elements[buffer->tailIndex] = newValue;
    // Copy new job values
    buffer->elements[buffer->tailIndex].duration = newValue.duration;
    buffer->elements[buffer->tailIndex].pagesToPrint = newValue.pagesToPrint;
    buffer->elements[buffer->tailIndex].id = newValue.id;

    buffer->tailIndex = (buffer->tailIndex + 1) % BUFFER_SIZE;
    // Increase the number of members
    buffer->numberOfMembers++;
}

PrintJob popFifoBuffer(FifoBuffer* buffer) {
    // Grab the element we will take
    PrintJob output = buffer->elements[buffer->headIndex];
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
