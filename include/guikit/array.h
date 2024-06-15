/**
 *  @file array.h
 *  @brief Array
 *
 *  Patater GUI Kit
 *  Created by Jaeden Amero on 2021-04-21.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef ARRAY_H
#define ARRAY_H

/* A dynamic array. API inspired by Sean Barrett's stretchy buffer
 * (nothings.org). */

#include "guikit/pmemory.h"
#include <stddef.h>

/*
 * Interface
 */

#define array_grow(a, n) ((a) = array_grow_impl((a), sizeof(*(a)), n))

#define array_push(a, val) ( \
    array_fit(a, 1), \
    (a)[array_to_header(a)->len++] = val \
)

#define array_pop(a) ((a)[--array_to_header(a)->len])

#define array_free(a) ((a) ? pfree(array_to_header(a)), (a) = NULL : 0)

static size_t array_len(const void *a);
static size_t array_cap(const void *a);



/*
 * Implementation
 */

/* LCOV_EXCL_START */
void arrayUnusedFunctionWarningEliminator(void)
{
    /* To avoid warnings like:
     *
     *   ../src/../include/guikit/array.h:69:15: error: unused function
     *   'array_cap' [-Werror,-Wunused-function]
     *
     *  cast the functions to void in a dummy function. We don't know of a
     *  better portable way to do this, as how to selectively turn off warnings
     *  is compiler-specific.
     */
    (void)array_cap;
    (void)array_len;
}
/* LCOV_EXCL_STOP */

static size_t max(size_t x, size_t y)
{
    return x > y ? x : y;
}

struct array_header {
    size_t len;
    size_t cap;
    char a[1]; /* Room for at least one element here, possibly more. */
};

#define array_to_header(a) ((struct array_header *)(a) - 1)
#define header_to_array(h) ((char *)(h) + sizeof(*h))

/* Return the length of the array (i.e. the number of elements it currently
 * contains) */
static size_t array_len(const void *a)
{
    if (a == NULL)
    {
        return 0;
    }

    return array_to_header(a)->len;
}

/* Return the capacity of the array (i.e. the maximum number of elements the
 * array can hold without resizing). */
static size_t array_cap(const void *a)
{
    if (a == NULL)
    {
        return 0;
    }

    return array_to_header(a)->cap;
}

/* Grow the array a, with element size s, to be able to hold n additional
 * elements. */
static void *array_grow_impl(void *a, size_t s, size_t n)
{
    struct array_header *h;
    size_t size = sizeof(*h);

    if (a)
    {
        h = array_to_header(a);

        /* Grow the array to 2x capacity, or just enough for the additional
         * elements, whichever is greater. This helps us avoid copying elements
         * within the array (which prealloc() does) more than once on average,
         * reducing the amortized run-time compexity of this grow function to
         * constant time. */
        /* Note that we are not using array_cap() or array_len() here, because
         * we already know a is non-null so we can skip the NULL checks that
         * array_cap() and array_len() include. */
        h->cap = max(2 * h->cap, h->len + n);
        size += h->cap * s;

        h = prealloc(h, size);
    }
    else
    {
        /* Allocate just enough space for the requested number of additional
         * elements. */
        size += n * s;
        h = pmalloc(size);
        h->len = 0;
        h->cap = n;
    }

    return header_to_array(h);
}

#define array_fit(a, n) \
    ((!(a) || array_len(a) + (n) > array_cap(a)) \
         ? (array_grow((a), (n))) \
         : 0)

#endif
