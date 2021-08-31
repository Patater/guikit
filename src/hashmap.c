/*
 *  hashmap.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-04.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/hashmap.h"
#include "guikit/pmemory.h"

static const size_t MIN_CAPACITY = 8;

struct hashmap
{
    size_t length;
    size_t num_collisions;
    size_t capacity;
    u32 (*keys)[2];
    ptrdiff_t *values;
};

struct hashmap *hashmap_alloc_cap(size_t capacity)
{
    struct hashmap *hashmap;

    hashmap = pmalloc(sizeof(*hashmap));
    hashmap->length = 0;
    hashmap->num_collisions = 0;
    hashmap->capacity = capacity;
    hashmap->keys = pcalloc(hashmap->capacity, sizeof(*hashmap->keys));
    hashmap->values = pcalloc(hashmap->capacity, sizeof(*hashmap->values));

    return hashmap;
}

struct hashmap *hashmap_alloc(void)
{
    return hashmap_alloc_cap(MIN_CAPACITY);
}

static void hashmap_grow(struct hashmap *hashmap, size_t capacity)
{
    size_t i;
    u32 (*old_keys)[2];
    ptrdiff_t *old_values;
    size_t old_capacity;

    old_capacity = hashmap->capacity;
    old_keys = hashmap->keys;
    old_values = hashmap->values;

    /* Allocate memory for the new keys and values. */
    hashmap->length = 0;
    hashmap->capacity = capacity;
    hashmap->keys = pcalloc(hashmap->capacity, sizeof(*hashmap->keys));
    hashmap->values = pcalloc(hashmap->capacity, sizeof(*hashmap->values));

    for (i = 0; i < old_capacity; ++i)
    {
        /* Add each previously occupied slot to the new map. */
        if (old_keys[i][0] != 0 ||
            old_keys[i][1] != 0)
        {
            hashmap_put(hashmap, old_keys[i], old_values[i]);
        }
    }

    /* Free the old hashmap memory. */
    pfree(old_keys);
    pfree(old_values);
}

void hashmap_free(struct hashmap *hashmap)
{
    if (hashmap)
    {
        pfree(hashmap->keys);
        pfree(hashmap->values);
    }
    pfree(hashmap);
}

ptrdiff_t hashmap_get(const struct hashmap *hashmap, const u32 hash[])
{
    size_t i;

    if (hashmap->length == 0)
    {
        /* Nothing inside. */
        return 0;
    }

    /* We are ignoring the upper 32-bits of hash for indexing purposes, as
     * they'd get masked off anyway when converting the hash to an index. */
    i = hash[0];

    for (;;)
    {
        /* Convert hash or out-of-bounds index to a valid index. */
        i &= hashmap->capacity - 1;

        if (hashmap->keys[i][0] == hash[0] &&
            hashmap->keys[i][1] == hash[1])
        {
            /* Found the key! */
            return hashmap->values[i];
        }
        else if (hashmap->keys[i][0] == 0 &&
                 hashmap->keys[i][1] == 0)
        {
            /* Found first empty slot. We can stop searching. */
            break;
        }
        /* Try the next slot. */
        ++i;
    }

    /* We've ended our scan and didn't find the key. */
    return 0;
}

void hashmap_put(struct hashmap *hashmap, const u32 hash[], ptrdiff_t value)
{
    size_t i;
    size_t initial_i;

    if (hashmap->length >= hashmap->capacity / 2)
    {
        /* Double the size of the map. Keep it to a power of 2 in size, so that
         * we can efficiently convert from a hash to an index with a mask
         * (bitwise anding with one less than the capacity). */
        /* Note that growing here will avoid infinite loops in hashmap_get() as
         * we'll always be guaranteed a bunch of empty space in the map. */
        hashmap_grow(hashmap, 2 * hashmap->capacity);
    }

    i = hash[0];
    initial_i = i & (hashmap->capacity - 1);

    for (;;)
    {
        /* Convert hash or out-of-bounds index to a valid index. */
        i &= hashmap->capacity - 1;

        if (hashmap->keys[i][0] == 0 && hashmap->keys[i][1] == 0)
        {
            break;
        }
        else if (hashmap->keys[i][0] == hash[0]
                 && hashmap->keys[i][1] == hash[1])
        {
            /* Found an existing index with same hash. As we use a decent
             * 64-bit hash, this is very likely to be the key we are looking
             * for. So, go ahead and update the value. */
            hashmap->values[i] = value;
            return;
        }
        /* Try the next slot. Take a note that we had to go to the next slot as
         * we encountered another value with similar hash (enough to want to go
         * into the same index). */
        ++i;
    }

    /* Found an empty slot. We can stop looking for one. Go ahead and
     * set the key and value. */
    hashmap->keys[i][0] =  hash[0];
    hashmap->keys[i][1] =  hash[1];
    hashmap->values[i] = value;
    ++hashmap->length;
    if (i != initial_i)
    {
        ++hashmap->num_collisions;
    }
}

void hashmap_del(struct hashmap *hashmap, const u32 hash[])
{
    size_t i;

    i = hash[0];

    for (;;)
    {
        /* Convert hash or out-of-bounds index to a valid index. */
        i &= hashmap->capacity - 1;

        if (hashmap->keys[i][0] == 0 && hashmap->keys[i][1] == 0)
        {
            /* Key not present, so nothing to do. */
            return;
        }
        else if (hashmap->keys[i][0] == hash[0]
                 && hashmap->keys[i][1] == hash[1])
        {
            /* Found an existing index with same hash. As we use a decent
             * 64-bit hash, this is very likely to be the key we are looking
             * for. */
             break;
        }
        /* Try the next slot. */
        ++i;
    }

    hashmap->keys[i][0] = 0;
    hashmap->keys[i][1] = 0;
    --hashmap->length;

    /* Re-add all subsequent occupied keys in the same probe range, as leaving
     * a gap where we removed the key will cause these subsequent entries to
     * never be found (as probing stops at first 0 hash and we just make the
     * removed entry have a zero hash). */
    ++i;
    for (;;)
    {
        u32 key[2];
        ptrdiff_t value;

        /* Convert hash or out-of-bounds index to a valid index. */
        i &= hashmap->capacity - 1;

        if (hashmap->keys[i][0] == 0 && hashmap->keys[i][1] == 0)
        {
            /* End of the probe range. We are done. */
            break;
        }

        key[0] = hashmap->keys[i][0];
        key[1] = hashmap->keys[i][1];
        value = hashmap->values[i];
        hashmap->keys[i][0] = 0;
        hashmap->keys[i][1] = 0;
        --hashmap->length;
        hashmap_put(hashmap, key, value);

        /* Keep going within the probe range. */
        ++i;
    }

    /* We've now re-added any entries within the same probe range. */
}

size_t hashmap_capacity(const struct hashmap *hashmap)
{
    return hashmap->capacity;
}

size_t hashmap_length(const struct hashmap *hashmap)
{
    return hashmap->length;
}

size_t hashmap_num_collisions(const struct hashmap *hashmap)
{
    return hashmap->num_collisions;
}
