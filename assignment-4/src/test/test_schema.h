//
// Created by Andrew on 2015-11-25.
//

#ifndef ASSIGNMENT_4_TEST_SCHEMA_H
#define ASSIGNMENT_4_TEST_SCHEMA_H

int ASSERT_EQUAL(char* nameA, char* nameB, char* testName) {
    return ASSERT_TRUE(strcmp(nameA, nameB) == 0, testName);
}

int ASSERT_EQUAL(int a, int b, char* testName) {
    return ASSERT_TRUE(a == b, testName);
}

int ASSERT_EQUAL(void* a, void* b, char* testName) {
    return ASSERT_TRUE(a == b, testName);
}

int ASSERT_TRUE(int boolean, char* testName) {
    if (boolean) {
        printf("%s: PASSED\n", testName);
        return 1;
    } else {
        printf("%s: FAILED\n", testName);
        return 0;
    }
}

#endif //ASSIGNMENT_4_TEST_SCHEMA_H
