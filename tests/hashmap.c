/*
 *  hashmap.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-04.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/hash.h"
#include "guikit/hashmap.h"
#include "test.h"

/* Mock hash */
void hash64(u32 out[], const char *buf, size_t len)
{
    if (len == sizeof("magic_key") - 1 && memcmp(buf, "magic_key", len) == 0)
    {
        out[0] = 0xA;
        return;
    }
    else if (len == sizeof("dead_key") - 1
             && memcmp(buf, "dead_key", len) == 0)
    {
        out[0] = 0xB;
        return;
    }
    else if (len == sizeof("costarring") - 1
             && memcmp(buf, "costarring", len) == 0)
    {
        out[0] = 0xC; /* collides with liquid */
        out[1] = 0x1;
        return;
    }
    else if (len == sizeof("liquid") - 1 && memcmp(buf, "liquid", len) == 0)
    {
        out[0] = 0xC; /* collides with costarring */
        out[1] = 0x2;
        return;
    }
    else if (len == sizeof("declinate") - 1
             && memcmp(buf, "declinate", len) == 0)
    {
        out[0] = 0xD; /* collides with macallums */
        out[1] = 0x1;
        return;
    }
    else if (len == sizeof("macallums") - 1
             && memcmp(buf, "macallums", len) == 0)
    {
        out[0] = 0xD; /* collides with declinate */
        out[1] = 0x2;
        return;
    }
    else if (len == sizeof("nacallumz") - 1
             && memcmp(buf, "nacallumz", len) == 0)
    {
        out[0] = 0xE;
        return;
    }
    else if (len == sizeof("parcel") - 1 && memcmp(buf, "parcel", len) == 0)
    {
        out[0] = 0xF;
        return;
    }
    else if (len == sizeof("frogman") - 1 && memcmp(buf, "frogman", len) == 0)
    {
        out[0] = 0x1;
        return;
    }
    else if (len == sizeof("badegg") - 1 && memcmp(buf, "badegg", len) == 0)
    {
        out[0] = 0x2;
        return;
    }
    else if (len == sizeof("triple_collision_1") - 1
             && memcmp(buf, "triple_collision_1", len) == 0)
    {
        out[0] = 0x3;
        out[1] = 0x1;
        return;
    }
    else if (len == sizeof("triple_collision_2") - 1
             && memcmp(buf, "triple_collision_2", len) == 0)
    {
        out[0] = 0x3;
        out[1] = 0x2;
        return;
    }
    else if (len == sizeof("triple_collision_3") - 1
             && memcmp(buf, "triple_collision_3", len) == 0)
    {
        out[0] = 0x3;
        out[1] = 0x3;
        return;
    }

    out[0] = 0xFFFFFFFFUL;
    out[1] = 0xFFFFFFFFUL;
}

static int hashmap_test_get_nonexistent(void)
{
    struct hashmap *hashmap;
    u32 key[2];
    ptrdiff_t value;

    /* Get non-existent from empty */
    hashmap = hashmap_alloc();
    hash64(key, "magic_key", sizeof("magic_key") - 1);
    value = hashmap_get(hashmap, key);
    TEST_EQ(value, 0);
    hashmap_free(hashmap);

    /* Get non-existent from non-collision bucket */
    hashmap = hashmap_alloc();
    hash64(key, "dead_key", sizeof("dead_key") - 1);
    hashmap_put(hashmap, key, 0xdeadbeefUL);
    hash64(key, "magic_key", sizeof("magic_key") - 1);
    value = hashmap_get(hashmap, key);
    TEST_EQ(value, 0);
    hashmap_free(hashmap);

    /* Get non-existent key from a bucket with a collision in it, where our
     * non-existent key has a different key length. */
    /* "costarring" collides with "liquid", which are each of different length.
     * https://softwareengineering.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed/145633#145633
     */
    hashmap = hashmap_alloc();
    hash64(key, "costarring", sizeof("costarring") - 1);
    hashmap_put(hashmap, key, 0x02UL);
    hash64(key, "liquid", sizeof("liquid") - 1);
    hashmap_put(hashmap, key, 0x03UL);
    hash64(key, "magic_key", sizeof("magic_key") - 1);
    value = hashmap_get(hashmap, key);
    TEST_EQ(value, 0);
    hashmap_free(hashmap);

    /* Get non-existent key from a bucket with a collision in it, where our key
     * has the same length as one of the colliding keys, but is a different key
     * key. */
    /* "declinate" collides with "macallums", which are both the same length.
     * https://softwareengineering.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed/145633#145633
     */
    hashmap = hashmap_alloc();
    hash64(key, "declinate", sizeof("declinate") - 1);
    hashmap_put(hashmap, key, 0x02UL);
    hash64(key, "macallums", sizeof("macallums") - 1);
    hashmap_put(hashmap, key, 0x03UL);
    hash64(key, "nacallumz", sizeof("nacallumz") - 1);
    value = hashmap_get(hashmap, key);
    TEST_EQ(value, 0);
    hashmap_free(hashmap);

    /* Get non-existent key from a bucket with a colliding key, where our key
     * collides, but has a different length. */
    hashmap = hashmap_alloc();
    hash64(key, "costarring", sizeof("costarring") - 1);
    hashmap_put(hashmap, key, 0x02UL);
    hash64(key, "liquid", sizeof("liquid") - 1);
    value = hashmap_get(hashmap, key);
    TEST_EQ(value, 0);
    hashmap_free(hashmap);

    /* Get non-existent key from a bucket with a colliding key, where our key
     * collides, and has the same length, but is a different key. */
    hashmap = hashmap_alloc();
    hash64(key, "declinate", sizeof("declinate") - 1);
    hashmap_put(hashmap, key, 0x02UL);
    hash64(key, "macallums", sizeof("macallums") - 1);
    value = hashmap_get(hashmap, key);
    TEST_EQ(value, 0);
    hashmap_free(hashmap);

    return 0;
}

static int hashmap_test_get_existent(void)
{
    struct hashmap *hashmap;
    u32 key[2];
    ptrdiff_t value;

    /* Get from non-collision bucket */
    hashmap = hashmap_alloc();
    hash64(key, "parcel", sizeof("parcel") - 1);
    hashmap_put(hashmap, key, 0x04UL);
    value = hashmap_get(hashmap, key);
    TEST_EQ(value, 0x00000004UL);
    hashmap_free(hashmap);

    /* Get from collision bucket first */
    hashmap = hashmap_alloc();
    hash64(key, "macallums", sizeof("macallums") - 1);
    hashmap_put(hashmap, key, 0x05UL);
    hash64(key, "declinate", sizeof("declinate") - 1);
    hashmap_put(hashmap, key, 0x06UL);
    hash64(key, "macallums", sizeof("macallums") - 1);
    value = hashmap_get(hashmap, key);
    TEST_EQ(value, 0x00000005UL);
    hashmap_free(hashmap);

    /* Get from collision bucket last */
    hashmap = hashmap_alloc();
    hash64(key, "declinate", sizeof("declinate") - 1);
    hashmap_put(hashmap, key, 0x07UL);
    hash64(key, "macallums", sizeof("macallums") - 1);
    hashmap_put(hashmap, key, 0x08UL);
    value = hashmap_get(hashmap, key);
    TEST_EQ(value, 0x00000008UL);
    hashmap_free(hashmap);

    return 0;
}

static int hashmap_test_put(void)
{
    struct hashmap *hashmap;
    u32 key[2];
    ptrdiff_t set_value;
    ptrdiff_t set_value1;
    ptrdiff_t set_value2;
    ptrdiff_t set_value3;
    ptrdiff_t got_value;
    ptrdiff_t got_value1;
    ptrdiff_t got_value2;
    ptrdiff_t got_value3;

    /* Put when non-existent. */
    hashmap = hashmap_alloc();
    TEST_EQU(hashmap_length(hashmap), 0);
    TEST_EQU(hashmap_num_collisions(hashmap), 0);
    set_value = 0x00000008UL;
    hash64(key, "frogman", sizeof("frogman") - 1);
    hashmap_put(hashmap, key, set_value);
    got_value = hashmap_get(hashmap, key);
    TEST_EQU(hashmap_length(hashmap), 1);
    TEST_EQU(hashmap_num_collisions(hashmap), 0);
    TEST_EQU(got_value, set_value);
    hashmap_free(hashmap);

    /* Put when non-existent in collision bucket, diff key len. */
    hashmap = hashmap_alloc();
    set_value = 0x00000009UL;
    set_value2 = 0xbabebeefUL;
    hash64(key, "costarring", sizeof("costarring") - 1);
    hashmap_put(hashmap, key, set_value2);
    hash64(key, "liquid", sizeof("liquid") - 1);
    hashmap_put(hashmap, key, set_value);
    TEST_EQU(hashmap_length(hashmap), 2);
    TEST_EQU(hashmap_num_collisions(hashmap), 1);
    got_value = hashmap_get(hashmap, key);
    hash64(key, "costarring", sizeof("costarring") - 1);
    got_value2 = hashmap_get(hashmap, key);
    TEST_EQU(got_value2, set_value2);
    TEST_EQU(got_value, set_value);
    hashmap_free(hashmap);

    /* Put when non-existent in collision bucket, diff key. */
    hashmap = hashmap_alloc();
    set_value = 0x0000000aUL;
    set_value2 = 0x50f7beefUL;
    hash64(key, "declinate", sizeof("declinate") - 1);
    hashmap_put(hashmap, key, set_value2);
    hash64(key, "macallums", sizeof("macallums") - 1);
    hashmap_put(hashmap, key, set_value);
    TEST_EQU(hashmap_length(hashmap), 2);
    TEST_EQU(hashmap_num_collisions(hashmap), 1);
    got_value = hashmap_get(hashmap, key);
    hash64(key, "declinate", sizeof("declinate") - 1);
    got_value2 = hashmap_get(hashmap, key);
    TEST_EQU(got_value2, set_value2);
    TEST_EQU(got_value, set_value);
    hashmap_free(hashmap);

    /* Put when existent in non-collision bucket. */
    hashmap = hashmap_alloc();
    set_value = 0x0000000bUL;
    hash64(key, "frogman", sizeof("frogman") - 1);
    hashmap_put(hashmap, key, 0xf00dfaceUL);
    hashmap_put(hashmap, key, set_value);
    TEST_EQU(hashmap_length(hashmap), 1);
    TEST_EQU(hashmap_num_collisions(hashmap), 0);
    got_value = hashmap_get(hashmap, key);
    TEST_EQU(got_value, set_value);
    hashmap_free(hashmap);

    /* Put when existent in collision bucket at front. */
    hashmap = hashmap_alloc();
    set_value = 0x0000000cUL;
    set_value2 = 0xc001d00dUL;
    hash64(key, "macallums", sizeof("macallums") - 1);
    hashmap_put(hashmap, key, 0xf00dfaceUL);
    hash64(key, "declinate", sizeof("declinate") - 1);
    hashmap_put(hashmap, key, set_value2);
    hash64(key, "macallums", sizeof("macallums") - 1);
    hashmap_put(hashmap, key, set_value);
    TEST_EQU(hashmap_length(hashmap), 2);
    TEST_EQU(hashmap_num_collisions(hashmap), 1);
    got_value = hashmap_get(hashmap, key);
    hash64(key, "declinate", sizeof("declinate") - 1);
    got_value2 = hashmap_get(hashmap, key);
    TEST_EQU(got_value, set_value);
    TEST_EQU(got_value2, set_value2);
    hashmap_free(hashmap);

    /* Put when existent in collision bucket at back. */
    hashmap = hashmap_alloc();
    set_value = 0x0000000dUL;
    set_value2 = 0xc001d00dUL;
    hash64(key, "declinate", sizeof("declinate") - 1);
    hashmap_put(hashmap, key, set_value2);
    hash64(key, "macallums", sizeof("macallums") - 1);
    hashmap_put(hashmap, key, 0xf00dfaceUL);
    hashmap_put(hashmap, key, set_value);
    TEST_EQU(hashmap_length(hashmap), 2);
    TEST_EQU(hashmap_num_collisions(hashmap), 1);
    got_value = hashmap_get(hashmap, key);
    hash64(key, "declinate", sizeof("declinate") - 1);
    got_value2 = hashmap_get(hashmap, key);
    TEST_EQU(got_value, set_value);
    TEST_EQU(got_value2, set_value2);
    hashmap_free(hashmap);

    /* Put when existent in collision bucket in middle. */
    hashmap = hashmap_alloc();
    set_value = 0x0000000dUL;
    set_value1 = 0xf00dfaceUL;
    set_value2 = 0xc001d00dUL;
    set_value3 = 0x5a5aa5a5UL;
    hash64(key, "triple_collision_1", sizeof("triple_collision_1") - 1);
    hashmap_put(hashmap, key, set_value1);
    hash64(key, "triple_collision_2", sizeof("triple_collision_2") - 1);
    hashmap_put(hashmap, key, set_value2);
    hash64(key, "triple_collision_3", sizeof("triple_collision_3") - 1);
    hashmap_put(hashmap, key, set_value3);
    TEST_EQU(hashmap_length(hashmap), 3);
    TEST_EQU(hashmap_num_collisions(hashmap), 2);
    hash64(key, "triple_collision_2", sizeof("triple_collision_2") - 1);
    hashmap_put(hashmap, key, set_value);
    hash64(key, "triple_collision_1", sizeof("triple_collision_1") - 1);
    got_value1 = hashmap_get(hashmap, key);
    hash64(key, "triple_collision_2", sizeof("triple_collision_2") - 1);
    got_value = hashmap_get(hashmap, key);
    hash64(key, "triple_collision_3", sizeof("triple_collision_3") - 1);
    got_value3 = hashmap_get(hashmap, key);
    TEST_EQU(got_value1, set_value1);
    TEST_EQU(got_value, set_value);
    TEST_EQU(got_value3, set_value3);
    hashmap_free(hashmap);

    return 0;
}

static int hashmap_test_del(void)
{
    struct hashmap *hashmap;
    u32 key[2];
    ptrdiff_t set_value;
    ptrdiff_t set_value2;
    ptrdiff_t got_value;
    ptrdiff_t got_value2;

    /* Remove when non-existent. */
    hashmap = hashmap_alloc();
    TEST_EQU(hashmap_length(hashmap), 0);
    TEST_EQU(hashmap_num_collisions(hashmap), 0);
    hash64(key, "badegg", sizeof("badegg") );
    hashmap_del(hashmap, key);
    TEST_EQU(hashmap_length(hashmap), 0);
    hashmap_free(hashmap);

    /* Remove when non-existent in collision bucket, diff key len. */
    /* There is only one entry in the bucket, but it's not the key we want to
     * remove. It collides with the key we want to remove, but because it's not
     * the same we test that it isn't removed. */
    hashmap = hashmap_alloc();
    set_value = 0xbabebeefUL;
    hash64(key, "costarring", sizeof("costarring") - 1);
    hashmap_put(hashmap, key, set_value);
    hash64(key, "liquid", sizeof("liquid") );
    hashmap_del(hashmap, key);
    TEST_EQU(hashmap_length(hashmap), 1);
    hash64(key, "costarring", sizeof("costarring") - 1);
    got_value = hashmap_get(hashmap, key);
    TEST_EQU(got_value, set_value);
    hashmap_free(hashmap);

    /* Remove when non-existent in collision bucket, diff key. */
    hashmap = hashmap_alloc();
    set_value = 0x50f7beefUL;
    hash64(key, "declinate", sizeof("declinate") - 1);
    hashmap_put(hashmap, key, set_value);
    hash64(key, "macallums", sizeof("macallums") );
    hashmap_del(hashmap, key);
    TEST_EQU(hashmap_length(hashmap), 1);
    hash64(key, "declinate", sizeof("declinate") - 1);
    got_value = hashmap_get(hashmap, key);
    TEST_EQU(got_value, set_value);
    hashmap_free(hashmap);

    /* Remove when existent in non-collision bucket. */
    hashmap = hashmap_alloc();
    set_value = 0xf00dfaceUL;
    hash64(key, "badegg", sizeof("badegg") - 1);
    hashmap_put(hashmap, key, set_value);
    hashmap_del(hashmap, key);
    TEST_EQU(hashmap_length(hashmap), 0);
    hashmap_free(hashmap);

    /* Remove when existent in collision bucket at front. */
    hashmap = hashmap_alloc();
    set_value = 0xf00dfaceUL;
    set_value2 = 0xc001d00dUL;
    hash64(key, "macallums", sizeof("macallums") - 1);
    hashmap_put(hashmap, key, set_value);
    hash64(key, "declinate", sizeof("declinate") - 1);
    hashmap_put(hashmap, key, set_value2);
    hash64(key, "macallums", sizeof("macallums") - 1);
    hashmap_del(hashmap, key);
    TEST_EQU(hashmap_length(hashmap), 1);
    TEST_EQU(hashmap_num_collisions(hashmap), 1);
    hash64(key, "declinate", sizeof("declinate") - 1);
    got_value2 = hashmap_get(hashmap, key);
    TEST_EQU(got_value2, set_value2);
    hashmap_free(hashmap);

    /* Remove when existent in collision bucket at back. */
    hashmap = hashmap_alloc();
    set_value = 0xf00dfaceUL;
    set_value2 = 0xc001d00dUL;
    hash64(key, "declinate", sizeof("declinate") - 1);
    hashmap_put(hashmap, key, set_value2);
    hash64(key, "macallums", sizeof("macallums") - 1);
    hashmap_put(hashmap, key, set_value);
    hashmap_del(hashmap, key);
    TEST_EQU(hashmap_length(hashmap), 1);
    TEST_EQU(hashmap_num_collisions(hashmap), 1);
    hash64(key, "declinate", sizeof("declinate") - 1);
    got_value2 = hashmap_get(hashmap, key);
    TEST_EQU(got_value2, set_value2);
    hashmap_free(hashmap);

    return 0;
}

const test_fn tests[] =
{
    hashmap_test_get_nonexistent,
    hashmap_test_get_existent,
    hashmap_test_put,
    hashmap_test_del,
    0
};
