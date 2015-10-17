/*
 * fifi_buffer.h
 *
 *  Created on: Oct 11, 2015
 *      Author: Andrew Fogarty
 */

#ifndef PRINT_JOB_H_
#define PRINT_JOB_H_


typedef struct print_job {
	int id;
	int duration;
	int pagesToPrint;
} PrintJob;


/**
 * Print the properties of a print job struct.
 */
void printPrintJob(PrintJob job) {
    printf("Print Job { id: %d, pages: %d, duration: %d }\n", job.id, job.pagesToPrint, job.duration);
}

#endif /* PRINT_JOB_H_ */
