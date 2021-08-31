/*
 *  hashmap.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-04.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/hashmap.h"
#include "guikit/pmemory.h"

static const size_t MIN_CAPACITY = 128;

struct hashmap
{
    size_t length;
    size_t num_collisions;
    size_t capacity;
    struct hashmap_node *bucket;
};

struct hashmap_node
{
    u32 hash[2];
    ptrdiff_t value;
    struct hashmap_node *next;
};

/* TODO: automatically grow the hashmap if the number of collisions exceeds
 * some threshold. Maybe if length exceeds about half the capacity of the
 * hashmap? */
/*static void hashmap_grow();*/

struct hashmap *hashmap_alloc(void)
{
    struct hashmap *hashmap;

    hashmap = pmalloc(sizeof(*hashmap));
    hashmap->length = 0;
    hashmap->num_collisions = 0;
    hashmap->capacity = MIN_CAPACITY;
    hashmap->bucket = pcalloc(hashmap->capacity, sizeof(*hashmap->bucket));

    return hashmap;
}

void hashmap_free(struct hashmap *hashmap)
{
    if (hashmap)
    {
        size_t i;

        for (i = 0; i < hashmap->capacity; i++)
        {
            struct hashmap_node *n = &hashmap->bucket[i];
            /* Free linked list of nodes. */
            n = n->next;
            while (n)
            {
                struct hashmap_node *next = n->next;
                pfree(n);
                n = next;
            }
        }
        pfree(hashmap->bucket);
    }
    pfree(hashmap);
}

static size_t hash_to_index(const u32 hash[], size_t capacity)
{
    return hash[0] % capacity;
}

ptrdiff_t hashmap_get(const struct hashmap *hashmap, const u32 hash[])
{
    size_t i;
    struct hashmap_node *n;

    i = hash_to_index(hash, hashmap->capacity);

    n = &hashmap->bucket[i];
    if (n->hash[0] == 0 && n->hash[1] == 0)
    {
        /* Nothing here */
        /* XXX Shall we return something for not available other than 0? */
        return 0;
    }

    /* Search linearly through the linked list of hashmap nodes for the
     * exact hash. */
    while (n)
    {
        struct hashmap_node *next = n->next;
        if (n->hash[0] == hash[0] && n->hash[1] == hash[1])
        {
            /* Found matching hash. */
            return n->value;
        }

        /* Didn't find a matching hash. Look at the next node. */
        n = next;
    }

    /* We've exhausted the bucket but didn't find the hash. */
    return 0;
}

void hashmap_put(struct hashmap *hashmap, const u32 hash[], ptrdiff_t value)
{
    size_t i;
    struct hashmap_node *n;

    i = hash_to_index(hash, hashmap->capacity);

    n = &hashmap->bucket[i];
    if (n->hash[0] == 0 && n->hash[1] == 0)
    {
        /* Fresh bucket. Add first entry to bucket. */
        hashmap->length += 1;
        n->hash[0] = hash[0];
        n->hash[1] = hash[1];
        n->value = value;
        n->next = NULL;

        return;
    }

    /* Search linearly through the linked list of hashmap nodes for the
     * exact hash. */
    for (;;)
    {
        struct hashmap_node *next = n->next;
        if (n->hash[0] == hash[0] && n->hash[1] == hash[1])
        {
            /* Found matching hash */
            n->value = value;
            return;
        }

        /* End of list; didn't find hash */
        if (n->next == NULL)
        {
            break;
        }

        n = next;
    }

    /* Went through entire bucket and didn't find entry. */
    /* Append to end of bucket. */
    hashmap->num_collisions += 1;
    hashmap->length += 1;
    n->next = pmalloc(sizeof(*n->next));
    n->next->hash[0] = hash[0];
    n->next->hash[1] = hash[1];
    n->next->value = value;
    n->next->next = NULL;

    return;
}

void hashmap_del(struct hashmap *hashmap, const u32 hash[])
{
    size_t i;
    struct hashmap_node *n;
    struct hashmap_node *prev;

    i = hash_to_index(hash, hashmap->capacity);

    n = &hashmap->bucket[i];
    if (n->hash[0] == 0 && n->hash[1] == 0)
    {
        /* Fresh, empty bucket. Nothing to do. */
        return;
    }

    /* Search linearly through the linked list of hashmap nodes for the
     * exact hash. */
    prev = NULL;
    for (;;)
    {
        struct hashmap_node *next = n->next;
        if (n->hash[0] == hash[0] && n->hash[1] == hash[1])
        {
            /* Found matching hash. */
            hashmap->length -= 1;
            if (prev)
            {
                prev->next = n->next;
                pfree(n);
            }
            else
            {
                /* The first entry in bucket is to be removed. */
                if (n->next)
                {
                    /* Copy the next node in and free the next node. TODO Make
                     * the bucket list a list of pointers, rather than an array
                     * of heads of linked lists. Then we can avoid this copy
                     * and just update the node pointers. */
                    *n = *n->next;
                    pfree(n->next);
                }
                else
                {
                    /* There is only one node in the bucket. */
                    n->hash[0] = 0;
                    n->hash[1] = 0;
                    n->value = 0;
                    n->next = NULL;
                }
            }
            return;
        }

        /* End of list; didn't find hash */
        if (n->next == NULL)
        {
            break;
        }

        /* Didn't find a matching hash. Look at the next node. */
        prev = n;
        n = next;
    }

    /* Went through entire bucket and didn't find entry. */
    return;
}

size_t hashmap_length(const struct hashmap *hashmap)
{
    return hashmap->length;
}

size_t hashmap_num_collisions(const struct hashmap *hashmap)
{
    return hashmap->num_collisions;
}
