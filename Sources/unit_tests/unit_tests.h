/**
 * @file unit_tests.h
 * @brief Unit Test Framework
 *
 * This file contains macros and utilities for performing unit tests.
 *
 * @author Jack Lukomski
 */

#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H

#include <stdio.h>

#define UNIT_TESTS 0

/**
 * @def ESP_ASSERT
 * @brief Macro for performing unit test assertions.
 *
 * This macro checks the condition and prints the test result.
 * If the condition is false, it prints the test failure message.
 *
 * @param name The name of the unit test.
 * @param condition The condition to be checked.
 * @param message The failure message to be printed.
 */
#define ESP_ASSERT(name, condition, message)                                   \
  {                                                                            \
    printf("Unit Test Name: %s, Test Result: ", name);                         \
    if (!(condition)) {                                                        \
      printf("Test Failed: %s\n", message);                                    \
    } else {                                                                   \
      printf("Test Passed!\n");                                                \
    }                                                                          \
  }

#endif /* UNIT_TESTS_H */
