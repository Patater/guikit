/*
 *  hashmap.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-04.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/hashmap.h"
#include "guikit/hash.h"
#include "guikit/pmemory.h"
#include <stdlib.h>
#include <string.h>

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
    size_t key_len;
    const char *key;
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

static size_t hash_to_index(u32 hash[], size_t capacity)
{
    return hash[0] % capacity;
}

ptrdiff_t hashmap_get(const struct hashmap *hashmap,
                      const char *key, size_t key_len)
{
    u32 h[2];
    size_t i;
    struct hashmap_node *n;

    hash64(h, key, key_len);
    i = hash_to_index(h, hashmap->capacity);

    n = &hashmap->bucket[i];
    if (n->key == NULL)
    {
        /* Nothing here */
        /* XXX Shall we return something for not available other than 0? */
        return 0;
    }

    /* Search linearly through the linked list of hashmap nodes for the
     * exact key. */
    while (n)
    {
        struct hashmap_node *next = n->next;
        if (n->key_len == key_len && memcmp(n->key, key, key_len) == 0)
        {
            /* Found matching key. */
            return n->value;
        }

        /* Didn't find a matching key. Look at the next node. */
        n = next;
    }

    /* We've exhausted the bucket but didn't find the key. */
    return 0;
}

void hashmap_set(struct hashmap *hashmap,
                 const char *key, size_t key_len, ptrdiff_t value)
{
    u32 h[2];
    size_t i;
    struct hashmap_node *n;

    hash64(h, key, key_len);
    i = hash_to_index(h, hashmap->capacity);

    n = &hashmap->bucket[i];
    if (n->key == NULL)
    {
        /* Fresh bucket. Add first entry to bucket. */
        hashmap->length += 1;
        n->key_len = key_len;
        n->key = key;
        n->hash[0] = h[0];
        n->hash[1] = h[1];
        n->value = value;
        n->next = NULL;

        return;
    }

    /* Search linearly through the linked list of hashmap nodes for the
     * exact key. */
    for (;;)
    {
        struct hashmap_node *next = n->next;
        if (n->key_len == key_len && memcmp(n->key, key, key_len) == 0)
        {
            /* Found matching key */
            n->value = value;
            return;
        }

        /* End of list; didn't find key */
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
    n->next->key_len = key_len;
    n->next->key = key;
    n->next->hash[0] = h[0];
    n->next->hash[1] = h[1];
    n->next->value = value;
    n->next->next = NULL;

    return;
}

void hashmap_remove(struct hashmap *hashmap,
                    const char *key, size_t key_len)
{
    u32 h[2];
    size_t i;
    struct hashmap_node *n;
    struct hashmap_node *prev;

    hash64(h, key, key_len);
    i = hash_to_index(h, hashmap->capacity);

    n = &hashmap->bucket[i];
    if (n->key == NULL)
    {
        /* Fresh, empty bucket. Nothing to do. */
        return;
    }

    /* Search linearly through the linked list of hashmap nodes for the
     * exact key. */
    prev = NULL;
    for (;;)
    {
        struct hashmap_node *next = n->next;
        if (n->key_len == key_len && memcmp(n->key, key, key_len) == 0)
        {
            /* Found matching key. */
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
                    n->key_len = 0;
                    n->key = NULL;
                    n->hash[0] = 0;
                    n->hash[1] = 0;
                    n->value = 0;
                    n->next = NULL;
                }
            }
            return;
        }

        /* End of list; didn't find key */
        if (n->next == NULL)
        {
            break;
        }

        /* Didn't find a matching key. Look at the next node. */
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
