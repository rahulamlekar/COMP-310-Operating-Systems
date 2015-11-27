#include <string.h>
#include <stdio.h>
#include "malloc/memory_allocation.h"


int main() {

    // Initial Malloc Test

    // Int tests
    int maxInt = 12;
    int* intPointers[maxInt];
    int i;
    int errors = 0;
    for (i = 0; i < maxInt; i++) {
        intPointers[i] = my_malloc(sizeof(int));
        *intPointers[i] = i;
    }
    // Test that the values are corrent
    for (i = 0; i < maxInt; i++) {
        if (*intPointers[i] != i) {
            printf("Error: int mismatch (%d, %d)\n", *intPointers[i], i);
            errors++;
        }
    }

//    // Free those values
    for (i = 0; i < maxInt; i++) {
        my_free(intPointers[i]);
    }

    // Re-assign and re-test them
    for (i = 0; i < maxInt; i++) {
        intPointers[i] = my_malloc(sizeof(int));
        *intPointers[i] = i;
    }
    // Test that the values are corrent
    for (i = 0; i < maxInt; i++) {
        if (*intPointers[i] != i) {
            printf("Error: int mismatch (%d, %d)\n", *intPointers[i], i);
            errors++;
        }
    }


    printf("Int test complete. %d errors.\n", errors);




    // String Tests

    int numStrings = 5;

    char* string[numStrings];
    string[0] = "This is the first test string.\n";
    string[1] = "This is the second test string.\n";
    string[2] = "This is the third string for testing and is also a little bit longer.\n";
    string[3] = "String 4 is short.\n";
    string[4] = "Fifth string's the charm!\n";

    char* test[numStrings];
    for (i = 0; i < numStrings; i++) {
        test[i] = my_malloc(strlen(string[i]));
        strcpy(test[i], string[i]);
    }

    for (i = 0; i < numStrings; i++) {
        printf("String test %d:  %s", i + 1, test[i]);
    }

    printf("\n/////////////////////\nBegin Free Test:\n/////////////////////\n");

    // Free Test

    // Free the second and fourth string
    my_free(test[1]);
    my_free(test[3]);


    // Try printing the rest again
    for (i = 0; i < numStrings; i++) {
        if (i != 1 && i != 3) {
            printf("String test %d:  %s", i + 1, test[i]);
        }
    }

    // Now we're going to malloc a bunch of bytes to

    int numSillyBytes = 256;
    void* sillytest[numSillyBytes];
    // Malloc a bunch of silly bytes
    for (i = 0; i < numSillyBytes; i++) {
        printf("i : %d\n", i);
        sillytest[i] = my_malloc(1);
        // Write 0 to this data to nuke everything
        memset(sillytest[i], '\0', 1);
    }
//    // Free half of them
//    for (i = 0; i < (numSillyBytes / 2); i++) {
//        memset(sillytest[i], '\0', 1);
//        my_free(sillytest[i]);
//    }

    // Malloc our strings again
//    test[1] = my_malloc(strlen(string[1]));
//    strcpy(test[1], string[1]);
//    test[3] = my_malloc(strlen(string[3]));
//    strcpy(test[3], string[3]);

//    // Free the other half of silly bytes
//    for (; i < numSillyBytes; i++) {
//        memset(sillytest[i], '\0', 1);
//        my_free(sillytest[i]);
//    }

//    // See if our strings still work!
//    for (i = 0; i < numStrings; i++) {
//        printf("String test %d:  %s", i + 1, test[i]);
//    }


    return 0;
}