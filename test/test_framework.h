#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>

#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        fprintf(stderr, "Assertion failed: %s == %s, %s:%d\n", #expected, #actual, __FILE__, __LINE__); \
        exit(1); \
    }

#define ASSERT_STR_EQ(expected, actual) \
    if (strcmp((expected), (actual)) != 0) { \
        fprintf(stderr, "Assertion failed: %s == %s, %s:%d\n", #expected, #actual, __FILE__, __LINE__); \
        exit(1); \
    }

#define RUN_TEST(test) \
    printf("Running %s...\n", #test); \
    test(); \
    printf("%s passed.\n", #test);

#endif // TEST_FRAMEWORK_H
