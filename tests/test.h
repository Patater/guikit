/*
 *  test.h
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-11.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>

/* ANSI Terminal Colors */
/* Values from https://en.wikipedia.org/wiki/ANSI_escape_code. */
enum
{
    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    WHITE = 7,
    FOREGROUND = 30,
    BACKGROUND = 40
};

#define COLOR_START "\x1b[1;%d;%dm"
#define COLOR_CONT "\x1b[K"
#define COLOR_END "\x1b[0m"
#define COLOR_FAIL FOREGROUND + WHITE, BACKGROUND + RED
#define COLOR_PASS FOREGROUND + WHITE, BACKGROUND + GREEN

#define TEST_FAIL() \
    do \
    { \
        return -1; \
    } while (0)

#define TEST_TRUE(cond) \
    do \
    { \
        int c = (int)(cond); \
        if (!(c)) \
        { \
            (void)("LCOV_EXCL_START"); \
            printf(COLOR_START __FILE__ \
                   ":%d" COLOR_CONT "\nTest failed: '%s' " \
                   "(expected true, was false)" COLOR_END "\n", \
                   COLOR_FAIL, __LINE__, #cond); \
            TEST_FAIL(); \
            (void)("LCOV_EXCL_STOP"); \
        } \
    } while (0)

#define TEST_FALSE(cond) \
    do \
    { \
        int c = (int)(cond); \
        if (c) \
        { \
            (void)("LCOV_EXCL_START"); \
            printf(COLOR_START __FILE__ \
                   ":%d" COLOR_CONT "\nTest failed: '%s' " \
                   "(expected false, was true)" COLOR_END "\n", \
                   COLOR_FAIL, __LINE__, #cond); \
            TEST_FAIL(); \
            (void)("LCOV_EXCL_STOP"); \
        } \
    } while (0)

#define TEST_EQ_T(a, b, type, ts) \
    do \
    { \
        type a_val = (type)(a); \
        type b_val = (type)(b); \
        if (!(a_val == b_val)) \
        { \
            (void)("LCOV_EXCL_START"); \
            printf(COLOR_START __FILE__ \
                   ":%d" COLOR_CONT "\nTest failed: '%s == %s' " \
                   "(%s=" #ts ", %s=" #ts ")" COLOR_END "\n", \
                   COLOR_FAIL, __LINE__, #a, #b, #a, a_val, #b, b_val); \
            TEST_FAIL(); \
            (void)("LCOV_EXCL_STOP"); \
        } \
    } while (0)

#define TEST_NE_T(a, b, type, ts) \
    do \
    { \
        type a_val = (type)(a); \
        type b_val = (type)(b); \
        if (!(a_val != b_val)) \
        { \
            (void)("LCOV_EXCL_START"); \
            printf(COLOR_START __FILE__ \
                   ":%d" COLOR_CONT "\nTest failed: '%s != %s' " \
                   "(%s=" #ts ", %s=" #ts ")" COLOR_END "\n", \
                   COLOR_FAIL, __LINE__, #a, #b, #a, a_val, #b, b_val); \
            TEST_FAIL(); \
            (void)("LCOV_EXCL_STOP"); \
        } \
    } while (0)

#define TEST_GT_T(a, b, type, ts) \
    do \
    { \
        type a_val = (type)(a); \
        type b_val = (type)(b); \
        if (!(a_val > b_val)) \
        { \
            (void)("LCOV_EXCL_START"); \
            printf(COLOR_START __FILE__ \
                   ":%d" COLOR_CONT "\nTest failed: '%s > %s' " \
                   "(%s=" #ts ", %s=" #ts ")" COLOR_END "\n", \
                   COLOR_FAIL, __LINE__, #a, #b, #a, a_val, #b, b_val); \
            TEST_FAIL(); \
            (void)("LCOV_EXCL_STOP"); \
        } \
    } while (0)

#define TEST_LT_T(a, b, type, ts) \
    do \
    { \
        type a_val = (type)(a); \
        type b_val = (type)(b); \
        if (!(a_val < b_val)) \
        { \
            (void)("LCOV_EXCL_START"); \
            printf(COLOR_START __FILE__ \
                   ":%d" COLOR_CONT "\nTest failed: '%s < %s' " \
                   "(%s=" #ts ", %s=" #ts ")" COLOR_END "\n", \
                   COLOR_FAIL, __LINE__, #a, #b, #a, a_val, #b, b_val); \
            TEST_FAIL(); \
            (void)("LCOV_EXCL_STOP"); \
        } \
    } while (0)

#define TEST_GE_T(a, b, type, ts) \
    do \
    { \
        type a_val = (type)(a); \
        type b_val = (type)(b); \
        if (!(a_val >= b_val)) \
        { \
            (void)("LCOV_EXCL_START"); \
            printf(COLOR_START __FILE__ \
                   ":%d" COLOR_CONT "\nTest failed: '%s >= %s' " \
                   "(%s=" #ts ", %s=" #ts ")" COLOR_END "\n", \
                   COLOR_FAIL, __LINE__, #a, #b, #a, a_val, #b, b_val); \
            TEST_FAIL(); \
            (void)("LCOV_EXCL_STOP"); \
        } \
    } while (0)

#define TEST_LE_T(a, b, type, ts) \
    do \
    { \
        type a_val = (type)(a); \
        type b_val = (type)(b); \
        if (!(a_val <= b_val)) \
        { \
            (void)("LCOV_EXCL_START"); \
            printf(COLOR_START __FILE__ \
                   ":%d" COLOR_CONT "\nTest failed: '%s <= %s' " \
                   "(%s=" #ts ", %s=" #ts ")" COLOR_END "\n", \
                   COLOR_FAIL, __LINE__, #a, #b, #a, a_val, #b, b_val); \
            TEST_FAIL(); \
            (void)("LCOV_EXCL_STOP"); \
        } \
    } while (0)

/* Signed comparison */
#define TEST_EQ(a, b) TEST_EQ_T(a, b, ptrdiff_t, %ld)
#define TEST_NE(a, b) TEST_NE_T(a, b, ptrdiff_t, %ld)
#define TEST_GT(a, b) TEST_GT_T(a, b, ptrdiff_t, %ld)
#define TEST_LT(a, b) TEST_LT_T(a, b, ptrdiff_t, %ld)
#define TEST_GE(a, b) TEST_GE_T(a, b, ptrdiff_t, %ld)
#define TEST_LE(a, b) TEST_LE_T(a, b, ptrdiff_t, %ld)

/* Unsigned comparison */
#define TEST_EQU(a, b) TEST_EQ_T(a, b, size_t, %lu)
#define TEST_EQX(a, b) TEST_EQ_T(a, b, size_t, 0x%lX)
#define TEST_NEU(a, b) TEST_NE_T(a, b, size_t, %lu)
#define TEST_GTU(a, b) TEST_GT_T(a, b, size_t, %lu)
#define TEST_LTU(a, b) TEST_LT_T(a, b, size_t, %lu)
#define TEST_GEU(a, b) TEST_GE_T(a, b, size_t, %lu)
#define TEST_LEU(a, b) TEST_LE_T(a, b, size_t, %lu)

/* Pointer comparison */
#define TEST_EQP(a, b) TEST_EQ_T(a, b, void *, %p)
#define TEST_NEP(a, b) TEST_NE_T(a, b, void *, %p)
#define TEST_GTP(a, b) TEST_GT_T(a, b, void *, %p)
#define TEST_LTP(a, b) TEST_LT_T(a, b, void *, %p)
#define TEST_GEP(a, b) TEST_GE_T(a, b, void *, %p)
#define TEST_LEP(a, b) TEST_LE_T(a, b, void *, %p)

/* String comparison */
#define TEST_EQS(a, b) \
    do \
    { \
        const char *a_val = (a); \
        const char *b_val = (b); \
        if (strcmp(a_val, b_val)) \
        { \
            (void)("LCOV_EXCL_START"); \
            printf(COLOR_START __FILE__ \
                   ":%d" COLOR_CONT "\nTest failed: '%s == %s' " \
                   "(%s=\"%s\", %s=\"%s\")" COLOR_END "\n", \
                   COLOR_FAIL, __LINE__, #a, #b, #a, a_val, #b, b_val); \
            TEST_FAIL(); \
            (void)("LCOV_EXCL_STOP"); \
        } \
    } while (0)

/* Memory comparison */
#define TEST_EQM(a, b, len) \
    do \
    { \
        const char *a_val = (a); \
        const char *b_val = (b); \
        if (memcmp(a_val, b_val, len)) \
        { \
            (void)("LCOV_EXCL_START"); \
            printf(COLOR_START __FILE__ \
                   ":%d" COLOR_CONT "\nTest failed: '%s == %s' " \
                   "(%s=\"%s\", %s=\"%s\")" COLOR_END "\n", \
                   COLOR_FAIL, __LINE__, #a, #b, #a, a_val, #b, b_val); \
            TEST_FAIL(); \
            (void)("LCOV_EXCL_STOP"); \
        } \
    } while (0)

typedef int (*test_fn)(void);

#endif
