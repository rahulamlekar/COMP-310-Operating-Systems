//
// Created by Andrew on 2015-11-25.
//

#include <stddef.h>
#include <stdio.h>
#include "malloc/structs/free_block.h"

int failedTests = 0;
int totalTests = 0;

/////////////////////////////////////////////////////////////
// Functions for simple testing
/////////////////////////////////////////////////////////////

int ASSERT_TRUE(int boolean, char *testName) {
    totalTests++;
    if (boolean) {
        printf("%s: PASSED\n", testName);
        return 0;
    } else {
        printf("%s: FAILED\n", testName);
        failedTests++;
        return 1;
    }
}

int ASSERT_EQUAL_STRING(char *nameA, char *nameB, char *testName) {
    return ASSERT_TRUE(strcmp(nameA, nameB) == 0, testName);
}

int ASSERT_EQUAL_INT(int a, int b, char *testName) {
    return ASSERT_TRUE(a == b, testName);
}

int ASSERT_EQUAL_PTR(void *a, void *b, char *testName) {
    return ASSERT_TRUE(a == b, testName);
}

/////////////////////////////////////////////////////////////
// Functions for testing units
/////////////////////////////////////////////////////////////

typedef struct test_free_block_struct {
    int size;
    void* prev;
    void* next;
    int data;
} FreeBlock;

typedef struct test_contiguous_free_blocks {
    struct test_free_block_struct first;
    struct test_free_block_struct second;
    struct test_free_block_struct third;
} ContiguousFreeBlocks;

typedef struct fake_double_free_blocks {
    int size;
    void* prev;
    void* next;
    int data;
    int size2;
    void* prev2;
    void* next2;
    int data2;
} FakeDoubleFreeBlocks;

/**
 * This is an on-stack version of the free block for testing purposes
 */
void test_free_block() {

    void* testPrev = (void*) 91599512;
    void* testNext = (void*) 1828241;


    ContiguousFreeBlocks blocks = {
            {
                    sizeof(int),
                    testPrev,
                    testNext,
                    2177421
            },
            {
                    sizeof(int),
                    NULL,
                    NULL,
                    818428
            },
            {
                    sizeof(int),
                    NULL,
                    NULL,
                    18428421
            }
    };


    ASSERT_EQUAL_INT(FreeBlock_getInternalSize(&blocks.first), sizeof(int), "Free Block Internal Size Test");
    ASSERT_EQUAL_PTR(FreeBlock_getPrev(&blocks.first), (void*) 91599512, "Free Block Previous Value Test");
    ASSERT_EQUAL_PTR(FreeBlock_getNext(&blocks.first), (void*) 1828241, "Free Block Next Value Test");


    FakeDoubleFreeBlocks doubleFreeBlocks = {
            sizeof(int),
            testPrev,
            testNext,
            {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'},
            sizeof(int),
            testPrev,
            testNext,
            {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'}
    };

    printf("%d\n", sizeof(int) + 2*sizeof(void*) + sizeof(char[9]));
    printf("%p, %p, %p\n", &blocks.first, FreeBlock_getNextContiguousBlock(&blocks.first), &blocks.second);
    ASSERT_EQUAL_PTR(FreeBlock_getNextContiguousBlock(&blocks.first), &blocks.second, "Free Block Next Contiguous Block Test");



}

void test_free_block_list() {

}


int main() {
    // Test the free block code
    test_free_block();

    printf("%d tests ran. %d failed.\n", totalTests, failedTests);
}