#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H

#include <stdio.h>

#define UNIT_TESTS 1

#define ESP_ASSERT(name, condition, message) { \
    printf("Unit Test Name: %s, Test Result: ", name); \
    if (!(condition)) { \
        printf("Test Failed: %s\n", message); \
    } else { \
        printf("Test Passed!\n"); \
    } \
}

#endif /* UNIT_TESTS_H */