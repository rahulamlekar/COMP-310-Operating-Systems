//
// Created by Andrew on 2015-11-22.
//

#ifndef ASSIGNMENT_4_MEMORY_ALLOCATION_H
#define ASSIGNMENT_4_MEMORY_ALLOCATION_H

void *my_malloc(int size);
extern  char *my_malloc_error;

void my_free(void *ptr);

void my_mallopt(int policy);

extern void my_mallinfo();

#endif //ASSIGNMENT_4_MEMORY_ALLOCATION_H
