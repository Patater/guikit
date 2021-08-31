/*
 *  intern.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-06.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/intern.h"
#include "guikit/array.h"
#include "guikit/hash.h"
#include "guikit/hashmap.h"
#include "guikit/pmemory.h"
#include "guikit/ptypes.h"
#include <stddef.h>
#include <string.h>

static struct hashmap *intern_map = NULL;
static int num_hits = 0;
static char **intern_array;

void intern_free(void)
{
    size_t len;
    size_t i;

    hashmap_free(intern_map);
    intern_map = NULL;

    len = array_len(intern_array);
    for (i = 0; i < len; i++)
    {
        pfree(intern_array[i]);
    }
    array_free(intern_array);

    num_hits = 0;
}

const char *intern(const char *s)
{
    size_t len;
    const char *interned;
    char *copy;
    u32 key[2];

    /* Lazy load the intern map */
    if (!intern_map)
    {
        intern_map = hashmap_alloc();
    }

    len = strlen(s);
    hash64(key, s, len);
    interned = (const char *)hashmap_get(intern_map, key);
    if (interned != 0)
    {
        /* Found previous copy */
        num_hits += 1;
        return interned;
    }

    /* Didn't find previous copy, so add it to the intern map. */
    /* Note that we own the memory for the string we are interning. We do this
     * so that one can intern strings they've made up on the fly (temporary
     * strings) without the intern storage disappearing out from under us.
     *
     * We track pointers to allocated memory in the intern array. This allows
     * us to free all the memory we've allocated without using the hash map.
     *
     * We don't provide a way to avoid string copying at the moment, although
     * this would be straightforward to implement given we are tracking
     * allocated memory. If we did not have a separate intern_array, we would
     * find it very hard to track such things as the pointers in the hash map's
     * values array may or may not be strings we've allocated. TODO add a
     * function to not copy-intern to enable callers to make more efficient use
     * of string storage. */
    copy = pmalloc(len + 1);
    strcpy(copy, s);
    array_push(intern_array, copy); /* Track allocated memory */
    hashmap_put(intern_map, key, (ptrdiff_t)copy);
    interned = copy;

    return interned;
}

int intern_num_hits(void)
{
    return num_hits;
}

int intern_num_interned(void)
{
    return array_len(intern_array);
}
