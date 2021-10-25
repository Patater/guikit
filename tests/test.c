/*
 *  test.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-05.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "test.h"
#include <stdio.h>

#define PUTT_TEST_FAIL 125
#define PUTT_TEST_PASS 0

const char *testName;

/* The list of tests will come from another file. We just run them. The list is
 * an array that is zero-terminated, so we know there should be at least one
 * element. */
extern const test_fn tests[1];

int main()
{
    int ret;
    int tests_run = 0;
    int tests_passed = 0;
    const test_fn *list = tests;

    while (*list)
    {
        ret = list[0]();
        if (ret >= 0)
        {
            ++tests_passed;
        }
        ++tests_run;
        ++list;
    }

    printf("%d of %d tests passed.\n", tests_passed, tests_run);

    return tests_passed == tests_run ? PUTT_TEST_PASS : PUTT_TEST_FAIL;
}
