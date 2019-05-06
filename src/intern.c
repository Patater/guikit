/*
 *  intern.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-06.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/intern.h"
#include "guikit/hashmap.h"
#include "guikit/pmemory.h"
#include "guikit/ptypes.h"
#include <stddef.h>
#include <string.h>

static struct hashmap *intern_map = NULL;
static int num_hits = 0;
static int num_interned = 0;

void intern_free(void)
{
    hashmap_free(intern_map);
    intern_map = NULL;

    num_hits = 0;
    num_interned = 0;
}

const char *intern(const char *s)
{
    size_t len;
    const char *interned;
    char *copy;

    /* Lazy load the intern map */
    if (!intern_map)
    {
        intern_map = hashmap_alloc();
    }

    len = strlen(s);
    interned = (const char *)hashmap_get(intern_map, s, len);
    if (interned != 0)
    {
        /* Found previous copy */
        num_hits += 1;
        return interned;
    }

    /* Didn't find previous copy, so add it to the intern map. */
    /* TODO who will own memory for the interned strings? Will we ever free
     * them? If we don't own the memory, what if the added string goes out of
     * scope? Determine if we want the interner to own the memory and make
     * copies of strings added to it. */
    /* Yes, we want the interner to own the memory. This is known as "copy
     * semantics" or "<something> containers"?. */
    /* If the strings are static, we shouldn't have to pay the price for a
     * copy, though. TODO add a function to not copy-intern and let the caller
     * decide what to do. */
    copy = pmalloc(len + 1);
    strcpy(copy, s);
    num_interned += 1;
    hashmap_set(intern_map, copy, len, (ptrdiff_t)copy);
    interned = copy;

    return interned;
}

int intern_num_hits(void)
{
    return num_hits;
}

int intern_num_interned(void)
{
    return num_interned;
}
