#include <string.h>
#include <stdio.h>
#include "malloc/memory_allocation.h"

int main() {

    char* string1 = "This is the first test string.\n";
    char* string2 = "This is the second test string.\n";

    char* test1 = my_malloc(sizeof(string1));
    strcpy(test1, string1);

    printf("String test:  %s\n", test1);

    void* test2 = my_malloc(21);

    int* intTest = my_malloc(sizeof(int));
    *intTest = 1884214;

    void* test3 = my_malloc(51);

    printf("intTest: %d\n", *intTest);

    // Delete the contents, and see if stuff gets messed up
    memset(test2, '\0', 21);
    memset(test3, '\0', 51);

    void* test4 = my_malloc(81);
    void* test5 = my_malloc(sizeof(string2));

    //strcpy(test5, string2);

    // Try to free
    my_free(test4);
    void* test6 = my_malloc(41);

    printf("String test:  %s\n", test1);
    //printf("String test2: %s", test5);


    return 0;
}