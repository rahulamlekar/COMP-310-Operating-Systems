#include "print_job.h"


#define BUFFER_SIZE 6

typedef struct fifo_buffer {
	PrintJob* elements[BUFFER_SIZE];
	int elementFull[BUFFER_SIZE];
	int headIndex;
	int tailIndex;
} FifoBuffer;

int pushFifoBuffer(FifoBuffer* buffer, PrintJob* newValue) {
	if (!buffer->elementFull[buffer->tailIndex]) {
		// We can proceed
		buffer->elementFull[buffer->tailIndex] = 1;
		buffer->elements[buffer->tailIndex] = newValue;
		buffer->tailIndex = (buffer->tailIndex + 1) % BUFFER_SIZE;
		// It worked!
		return 1;
	}

	// It didn't work
	return 0;
}

PrintJob* popFifoBuffer(FifoBuffer* buffer) {
	// Grab the element we will take
	PrintJob* output = buffer->elements[buffer->headIndex];
	// Mark it as empty
	buffer->elementFull[buffer->headIndex] = 0;
	// Move the index along
	buffer->headIndex = (buffer->headIndex + 1) % BUFFER_SIZE;
	// Return the value
	return output;
}

