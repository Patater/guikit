/*
 *  pmemory.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-04.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/pmemory.h"
#include "guikit/panic.h"
#include <stdlib.h>

void *pmalloc(size_t size)
{
    void *ptr = malloc(size);
    /* LCOV_EXCL_START */
    if (ptr == NULL)
    {
        panic("%s: Out of memory", "pmalloc");
    }
    /* LCOV_EXCL_STOP */

    return ptr;
}

void *pcalloc(size_t number, size_t size)
{
    void *ptr = calloc(number, size);
    /* LCOV_EXCL_START */
    if (ptr == NULL)
    {
        panic("%s: Out of memory", "pcalloc");
    }
    /* LCOV_EXCL_STOP */

    return ptr;
}

void *prealloc(void *ptr, size_t size)
{
    ptr = realloc(ptr, size);
    /* LCOV_EXCL_START */
    if (ptr == NULL)
    {
        panic("%s: Out of memory", "prealloc");
    }
    /* LCOV_EXCL_STOP */

    return ptr;
}

void pfree(void *ptr)
{
    free(ptr);
}
