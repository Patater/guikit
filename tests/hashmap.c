/*
 *  hashmap.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-04.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/hashmap.h"
#include "test.h"

static int hashmap_test_get_nonexistent(void)
{
    struct hashmap *hashmap;
    ptrdiff_t value;

    /* Get non-existent from empty */
    hashmap = hashmap_alloc();
    value = hashmap_get(hashmap, "magic_key", sizeof("magic_key") - 1);
    TEST_EQ(value, 0);
    hashmap_free(hashmap);

    /* Get non-existent from non-collision bucket */
    hashmap = hashmap_alloc();
    hashmap_set(hashmap, "dead_key", sizeof("dead_key") - 1, 0xdeadbeefUL);
    value = hashmap_get(hashmap, "magic_key", sizeof("magic_key") - 1);
    TEST_EQ(value, 0);
    hashmap_free(hashmap);

    /* Get non-existent key from a bucket with a collision in it, where our
     * non-existent key has a different key length. */
    /* "costarring" collides with "liquid", which are each of different length.
     * https://softwareengineering.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed/145633#145633
     */
    hashmap = hashmap_alloc();
    hashmap_set(hashmap, "costarring", sizeof("costarring") - 1, 0x02UL);
    hashmap_set(hashmap, "liquid", sizeof("liquid") - 1, 0x03UL);
    value = hashmap_get(hashmap, "magic_key", sizeof("magic_key") - 1);
    TEST_EQ(value, 0);
    hashmap_free(hashmap);

    /* Get non-existent key from a bucket with a collision in it, where our key
     * has the same length as one of the colliding keys, but is a different key
     * key. */
    /* "declinate" collides with "macallums", which are both the same length.
     * https://softwareengineering.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed/145633#145633
     */
    hashmap = hashmap_alloc();
    hashmap_set(hashmap, "declinate", sizeof("declinate") - 1, 0x02UL);
    hashmap_set(hashmap, "macallums", sizeof("macallums") - 1, 0x03UL);
    value = hashmap_get(hashmap, "nacallumz", sizeof("nacallumz") - 1);
    TEST_EQ(value, 0);
    hashmap_free(hashmap);

    /* Get non-existent key from a bucket with a colliding key, where our key
     * collides, but has a different length. */
    hashmap = hashmap_alloc();
    hashmap_set(hashmap, "costarring", sizeof("costarring") - 1, 0x02UL);
    value = hashmap_get(hashmap, "liquid", sizeof("liquid") - 1);
    TEST_EQ(value, 0);
    hashmap_free(hashmap);

    /* Get non-existent key from a bucket with a colliding key, where our key
     * collides, and has the same length, but is a different key. */
    hashmap = hashmap_alloc();
    hashmap_set(hashmap, "declinate", sizeof("declinate") - 1, 0x02UL);
    value = hashmap_get(hashmap, "macallums", sizeof("macallums") - 1);
    TEST_EQ(value, 0);
    hashmap_free(hashmap);

    return 0;
}

static int hashmap_test_get_existent(void)
{
    struct hashmap *hashmap;
    ptrdiff_t value;

    /* Get from non-collision bucket */
    hashmap = hashmap_alloc();
    hashmap_set(hashmap, "parcel", sizeof("parcel") - 1, 0x04UL);
    value = hashmap_get(hashmap, "parcel", sizeof("parcel") - 1);
    TEST_EQ(value, 0x00000004UL);
    hashmap_free(hashmap);

    /* Get from collision bucket first */
    hashmap = hashmap_alloc();
    hashmap_set(hashmap, "macallums", sizeof("macallums") - 1, 0x05UL);
    hashmap_set(hashmap, "declinate", sizeof("declinate") - 1, 0x06UL);
    value = hashmap_get(hashmap, "macallums", sizeof("macallums") - 1);
    TEST_EQ(value, 0x00000005UL);
    hashmap_free(hashmap);

    /* Get from collision bucket last */
    hashmap = hashmap_alloc();
    hashmap_set(hashmap, "declinate", sizeof("declinate") - 1, 0x07UL);
    hashmap_set(hashmap, "macallums", sizeof("macallums") - 1, 0x08UL);
    value = hashmap_get(hashmap, "macallums", sizeof("macallums") - 1);
    TEST_EQ(value, 0x00000008UL);
    hashmap_free(hashmap);

    return 0;
}

static int hashmap_test_set(void)
{
    struct hashmap *hashmap;
    ptrdiff_t set_value;
    ptrdiff_t set_value2;
    ptrdiff_t got_value;
    ptrdiff_t got_value2;

    /* Set when non-existent. */
    hashmap = hashmap_alloc();
    TEST_EQU(hashmap_length(hashmap), 0);
    TEST_EQU(hashmap_num_collisions(hashmap), 0);
    set_value = 0x00000008UL;
    hashmap_set(hashmap, "frogman", sizeof("frogman") - 1, set_value);
    got_value = hashmap_get(hashmap, "frogman", sizeof("frogman") - 1);
    TEST_EQU(hashmap_length(hashmap), 1);
    TEST_EQU(hashmap_num_collisions(hashmap), 0);
    TEST_EQU(got_value, set_value);
    hashmap_free(hashmap);

    /* Set when non-existent in collision bucket, diff key len. */
    hashmap = hashmap_alloc();
    set_value = 0x00000009UL;
    set_value2 = 0xbabebeefUL;
    hashmap_set(hashmap, "costarring", sizeof("costarring") - 1, set_value2);
    hashmap_set(hashmap, "liquid", sizeof("liquid") - 1, set_value);
    TEST_EQU(hashmap_length(hashmap), 2);
    TEST_EQU(hashmap_num_collisions(hashmap), 1);
    got_value = hashmap_get(hashmap, "liquid", sizeof("liquid") - 1);
    got_value2 = hashmap_get(hashmap, "costarring", sizeof("costarring") - 1);
    TEST_EQU(got_value2, set_value2);
    TEST_EQU(got_value, set_value);
    hashmap_free(hashmap);

    /* Set when non-existent in collision bucket, diff key. */
    hashmap = hashmap_alloc();
    set_value = 0x0000000aUL;
    set_value2 = 0x50f7beefUL;
    hashmap_set(hashmap, "declinate", sizeof("declinate") - 1, set_value2);
    hashmap_set(hashmap, "macallums", sizeof("macallums") - 1, set_value);
    TEST_EQU(hashmap_length(hashmap), 2);
    TEST_EQU(hashmap_num_collisions(hashmap), 1);
    got_value = hashmap_get(hashmap, "macallums", sizeof("macallums") - 1);
    got_value2 = hashmap_get(hashmap, "declinate", sizeof("declinate") - 1);
    TEST_EQU(got_value2, set_value2);
    TEST_EQU(got_value, set_value);
    hashmap_free(hashmap);

    /* Set when existent in non-collision bucket. */
    hashmap = hashmap_alloc();
    set_value = 0x0000000bUL;
    hashmap_set(hashmap, "frogman", sizeof("frogman") - 1, 0xf00dfaceUL);
    hashmap_set(hashmap, "frogman", sizeof("frogman") - 1, set_value);
    TEST_EQU(hashmap_length(hashmap), 1);
    TEST_EQU(hashmap_num_collisions(hashmap), 0);
    got_value = hashmap_get(hashmap, "frogman", sizeof("frogman") - 1);
    TEST_EQU(got_value, set_value);
    hashmap_free(hashmap);

    /* Set when existent in collision bucket at front. */
    hashmap = hashmap_alloc();
    set_value = 0x0000000cUL;
    set_value2 = 0xc001d00dUL;
    hashmap_set(hashmap, "macallums", sizeof("macallums") - 1, 0xf00dfaceUL);
    hashmap_set(hashmap, "declinate", sizeof("declinate") - 1, set_value2);
    hashmap_set(hashmap, "macallums", sizeof("macallums") - 1, set_value);
    TEST_EQU(hashmap_length(hashmap), 2);
    TEST_EQU(hashmap_num_collisions(hashmap), 1);
    got_value = hashmap_get(hashmap, "macallums", sizeof("macallums") - 1);
    got_value2 = hashmap_get(hashmap, "declinate", sizeof("declinate") - 1);
    TEST_EQU(got_value, set_value);
    TEST_EQU(got_value2, set_value2);
    hashmap_free(hashmap);

    /* Set when existent in collision bucket at back. */
    hashmap = hashmap_alloc();
    set_value = 0x0000000dUL;
    set_value2 = 0xc001d00dUL;
    hashmap_set(hashmap, "declinate", sizeof("declinate") - 1, set_value2);
    hashmap_set(hashmap, "macallums", sizeof("macallums") - 1, 0xf00dfaceUL);
    hashmap_set(hashmap, "macallums", sizeof("macallums") - 1, set_value);
    TEST_EQU(hashmap_length(hashmap), 2);
    TEST_EQU(hashmap_num_collisions(hashmap), 1);
    got_value = hashmap_get(hashmap, "macallums", sizeof("macallums") - 1);
    got_value2 = hashmap_get(hashmap, "declinate", sizeof("declinate") - 1);
    TEST_EQU(got_value, set_value);
    TEST_EQU(got_value2, set_value2);
    hashmap_free(hashmap);

    return 0;
}

static int hashmap_test_remove(void)
{
    struct hashmap *hashmap;
    ptrdiff_t set_value;
    ptrdiff_t set_value2;
    ptrdiff_t got_value;
    ptrdiff_t got_value2;

    /* Remove when non-existent. */
    hashmap = hashmap_alloc();
    TEST_EQU(hashmap_length(hashmap), 0);
    TEST_EQU(hashmap_num_collisions(hashmap), 0);
    hashmap_remove(hashmap, "badegg", sizeof("badegg") - 1);
    TEST_EQU(hashmap_length(hashmap), 0);
    hashmap_free(hashmap);

    /* Remove when non-existent in collision bucket, diff key len. */
    /* There is only one entry in the bucket, but it's not the key we want to
     * remove. It collides with the key we want to remove, but because it's not
     * the same we test that it isn't removed. */
    hashmap = hashmap_alloc();
    set_value = 0xbabebeefUL;
    hashmap_set(hashmap, "costarring", sizeof("costarring") - 1, set_value);
    hashmap_remove(hashmap, "liquid", sizeof("liquid") - 1);
    TEST_EQU(hashmap_length(hashmap), 1);
    got_value = hashmap_get(hashmap, "costarring", sizeof("costarring") - 1);
    TEST_EQU(got_value, set_value);
    hashmap_free(hashmap);

    /* Remove when non-existent in collision bucket, diff key. */
    hashmap = hashmap_alloc();
    set_value = 0x50f7beefUL;
    hashmap_set(hashmap, "declinate", sizeof("declinate") - 1, set_value);
    hashmap_remove(hashmap, "macallums", sizeof("macallums") - 1);
    TEST_EQU(hashmap_length(hashmap), 1);
    got_value = hashmap_get(hashmap, "declinate", sizeof("declinate") - 1);
    TEST_EQU(got_value, set_value);
    hashmap_free(hashmap);

    /* Remove when existent in non-collision bucket. */
    hashmap = hashmap_alloc();
    set_value = 0xf00dfaceUL;
    hashmap_set(hashmap, "badegg", sizeof("badegg") - 1, set_value);
    hashmap_remove(hashmap, "badegg", sizeof("badegg") - 1);
    TEST_EQU(hashmap_length(hashmap), 0);
    hashmap_free(hashmap);

    /* Remove when existent in collision bucket at front. */
    hashmap = hashmap_alloc();
    set_value = 0xf00dfaceUL;
    set_value2 = 0xc001d00dUL;
    hashmap_set(hashmap, "macallums", sizeof("macallums") - 1, set_value);
    hashmap_set(hashmap, "declinate", sizeof("declinate") - 1, set_value2);
    hashmap_remove(hashmap, "macallums", sizeof("macallums") - 1);
    TEST_EQU(hashmap_length(hashmap), 1);
    TEST_EQU(hashmap_num_collisions(hashmap), 1);
    got_value2 = hashmap_get(hashmap, "declinate", sizeof("declinate") - 1);
    TEST_EQU(got_value2, set_value2);
    hashmap_free(hashmap);

    /* Remove when existent in collision bucket at back. */
    hashmap = hashmap_alloc();
    set_value = 0xf00dfaceUL;
    set_value2 = 0xc001d00dUL;
    hashmap_set(hashmap, "declinate", sizeof("declinate") - 1, set_value2);
    hashmap_set(hashmap, "macallums", sizeof("macallums") - 1, set_value);
    hashmap_remove(hashmap, "macallums", sizeof("macallums") - 1);
    TEST_EQU(hashmap_length(hashmap), 1);
    TEST_EQU(hashmap_num_collisions(hashmap), 1);
    got_value2 = hashmap_get(hashmap, "declinate", sizeof("declinate") - 1);
    TEST_EQU(got_value2, set_value2);
    hashmap_free(hashmap);

    /* Set when existent in collision bucket in middle. */
    /* This requires a triple collision, which we don't have available
     * currently... */
    #if 0
    hashmap = hashmap_alloc();
    hashmap->bucket[i].key_len = sizeof("collision") - 1;
    hashmap->bucket[i].key = "collision";
    hashmap->bucket[i].hash = h;
    hashmap->bucket[i].value = 0xc001d00dUL;
    hashmap->bucket[i].next = pmalloc(sizeof(*hashmap->bucket[i].next));
    hashmap->bucket[i].next->key_len = sizeof("badegg") - 1;
    hashmap->bucket[i].next->key = "badegg";
    hashmap->bucket[i].next->hash = h;
    hashmap->bucket[i].next->value = 0x0badbeefUL;
    hashmap->bucket[i].next->next = pmalloc(sizeof(*hashmap->bucket[i].next));
    hashmap->bucket[i].next->next->key_len = sizeof("frogman") - 1;
    hashmap->bucket[i].next->next->key = "frogman";
    hashmap->bucket[i].next->next->hash = h;
    hashmap->bucket[i].next->next->value = 0xf00dfaceUL;
    hashmap->bucket[i].next->next->next = NULL;
    hashmap->length = 3;
    hashmap->num_collisions = 2;
    hashmap_remove(hashmap, "badegg", sizeof("badegg") - 1);
    TEST_EQU(hashmap->length, 2);
    TEST_EQU(hashmap->num_collisions, 2);
    TEST_EQU(hashmap->bucket[i].value, 0xc001d00dUL);
    TEST_NEP(hashmap->bucket[i].next, NULL);
    TEST_EQU(hashmap->bucket[i].next->value, 0xf00dfaceUL);
    TEST_EQU(hashmap->bucket[i].next->next, NULL);
    hashmap_free(hashmap);
    #endif

    return 0;
}

const test_fn tests[] =
{
    hashmap_test_get_nonexistent,
    hashmap_test_get_existent,
    hashmap_test_set,
    hashmap_test_remove,
    0
};
