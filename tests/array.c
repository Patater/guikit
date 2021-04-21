/*
 *  array.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2020-04-21.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/array.h"
#include "test.h"

static int test_null_array_len_0(void)
{
    long *a = NULL;

    TEST_EQ(array_len(a), 0);

    return 0;
}

static int test_null_array_cap_0(void)
{
    long *a = NULL;

    TEST_EQ(array_cap(a), 0);

    return 0;
}

static int test_grow_1(void)
{
    long *a = NULL;

    array_grow(a, 1);

    TEST_NEP(a, NULL);
    TEST_EQ(array_len(a), 0);
    TEST_GT(array_cap(a), 0);

    return 0;
}

static int test_push_1(void)
{
    long *a = NULL;
    long val = 1234;

    array_push(a, val);

    TEST_EQ(a[0], val);
    TEST_EQ(array_len(a), 1);
    TEST_GT(array_cap(a), 0);

    return 0;
}

static int test_pop_1(void)
{
    long *a = NULL;
    long val = 1234;
    long got;

    array_push(a, val);
    got = array_pop(a);

    TEST_EQ(got, val);
    TEST_EQ(array_len(a), 0);
    TEST_GT(array_cap(a), 0);

    return 0;
}

static int test_grow_2(void)
{
    long *a = NULL;

    array_grow(a, 2);

    TEST_NEP(a, NULL);
    TEST_EQ(array_len(a), 0);
    TEST_GE(array_cap(a), 2);

    return 0;
}

static int test_push_2(void)
{
    long *a = NULL;
    long val[] = { 1234, 5678 };

    array_push(a, val[0]);
    array_push(a, val[1]);

    TEST_EQ(a[0], val[0]);
    TEST_EQ(a[1], val[1]);
    TEST_EQ(array_len(a), 2);
    TEST_GE(array_cap(a), 2);

    return 0;
}

static int test_pop_2(void)
{
    long *a = NULL;
    long val[] = { 1234, 5678 };
    long got;

    array_push(a, val[0]);
    array_push(a, val[1]);

    got = array_pop(a);
    TEST_EQ(got, val[1]);

    got = array_pop(a);
    TEST_EQ(got, val[0]);

    TEST_EQ(array_len(a), 0);
    TEST_GT(array_cap(a), 0);

    return 0;
}

static int test_amortized_constant_growth(void)
{
    /* Test that allocation are amortized constant time. To do this, we have to
     * expect the method to accomplish this is to double the array size when
     * more space is needed. If the method to accomplish amortized constant
     * time allocation changes, this test will need changing as well. */
    long *a = NULL;
    long val = 1234;

    array_push(a, val);
    array_push(a, val);
    array_push(a, val);

    TEST_EQ(array_len(a), 3);
    TEST_EQ(array_cap(a), 4);

    return 0;
}

static int test_null_array_free(void)
{
    long *a = NULL;

    array_free(a);

    TEST_EQP(a, NULL);

    return 0;
}

static int test_free(void)
{
    long *a = NULL;
    long val = 1234;

    array_push(a, val);

    TEST_NEP(a, NULL);

    array_free(a);

    TEST_EQP(a, NULL);

    return 0;
}

const test_fn tests[] =
{
    test_null_array_len_0,
    test_null_array_cap_0,
    test_grow_1,
    test_push_1,
    test_pop_1,
    test_grow_2,
    test_push_2,
    test_pop_2,
    test_amortized_constant_growth,
    test_null_array_free,
    test_free,
    0
};
