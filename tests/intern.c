/*
 *  intern.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-06.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/intern.h"
#include "test.h"

static int test_initial_state(void)
{
    intern_free();

    TEST_EQ(intern_num_hits(), 0);
    TEST_EQ(intern_num_interned(), 0);

    return 0;
}

static int test_after_first_string_added(void)
{
    const char *oldstr1;

    intern_free();

    /* Add first string. */
    oldstr1 = intern("hello");

    TEST_EQ(intern_num_hits(), 0);
    TEST_EQ(intern_num_interned(), 1);
    TEST_EQS(oldstr1, "hello");

    return 0;
}

static int test_after_same_string_added(void)
{
    const char *oldstr1;
    const char *newstr1;

    intern_free();

    /* Add same string. */
    oldstr1 = intern("hello");
    newstr1 = intern("hello");

    TEST_EQ(intern_num_hits(), 1);
    TEST_EQ(intern_num_interned(), 1);
    TEST_EQS(newstr1, "hello");
    TEST_EQP(newstr1, oldstr1);

    return 0;
}

static int test_after_different_string_added(void)
{
    const char *oldstr1;
    const char *newstr1;
    const char *newstr2;

    intern_free();

    /* Add different string. */
    oldstr1 = intern("hello");
    newstr1 = intern("hello");
    newstr2 = intern("world");

    TEST_EQ(intern_num_hits(), 1);
    TEST_EQ(intern_num_interned(), 2);
    TEST_EQP(newstr1, oldstr1);
    TEST_EQS(newstr2, "world");
    TEST_NEP(newstr2, oldstr1);

    return 0;
}

static int test_copy_semantics(void)
{
    static const char *base = "hello";
    const char *copy;

    intern_free();

    /* Add a string */
    copy = intern(base);

    /* Ensure that the copy returned is different memory than the base */
    TEST_NEP(copy, base);

    return 0;
}

static int test_copy_semantics_value(void)
{
    const char *copy;

    intern_free();

    {
        char base[] = "hello";
        copy = intern(base);

        TEST_NEP(copy, base);

        /* Ensure that base goes out of scope here by ending the block. */
    }

    /* Ensure that the copy is a true copy. This may crash if the memory used
     * for the base went out of scope. */
    TEST_EQS(copy, "hello");

    return 0;
}

const test_fn tests[] =
{
    test_initial_state,
    test_after_first_string_added,
    test_after_same_string_added,
    test_after_different_string_added,
    test_copy_semantics,
    test_copy_semantics_value,
    0
};
