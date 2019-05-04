/*
 *  hash.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-04.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/hash.h"
#include "test.h"

static int hash_test_chongo(void)
{
    unsigned long h;
    char test[] = "chongo <Landon Curt Noll> /\\../\\";

    h = hash(test, sizeof(test) - 1);
    TEST_EQU(h, 0x9a4e92e6UL);

    return 0;
}

static int hash_test_deadbeef(void)
{
    unsigned long h;
    unsigned char test[] = {0xde, 0xad, 0xbe, 0xef, 0x00, 0xa5, 0x5a};

    h = hash(test, 0);
    TEST_EQU(h, 0x811c9dc5UL);

    h = hash(test, 1);
    TEST_EQU(h, 0x5b0b5181UL);

    h = hash(test, 2);
    TEST_EQU(h, 0x7ed0c844UL);

    h = hash(test, 3);
    TEST_EQU(h, 0x9cac618eUL);

    h = hash(test, 4);
    TEST_EQU(h, 0x045d4bb3UL);

    h = hash(test, 5);
    TEST_EQU(h, 0x91de2ac9UL);

    h = hash(test, 6);
    TEST_EQU(h, 0x0cbcc804UL);

    h = hash(test, 7);
    TEST_EQU(h, 0x6b2f6bfaUL);

    return 0;
}

const test_fn tests[] =
{
    hash_test_deadbeef,
    hash_test_chongo,
    0
};
