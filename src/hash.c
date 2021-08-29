/*
 *  hash.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-04.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/hash.h"

/* FNV-1a algorithm description from
 * http://www.isthe.com/chongo/tech/comp/fnv/index.html#FNV-param */
static u32 hash_fnv1a_32(const char *buf, size_t len)
{
    /* FNV-1a parameters for a 32-bit hash */
    const u32 PRIME_32 = 16777619UL;
    const u32 OFFSET_32 = 2166136261UL;

    u32 h = OFFSET_32;
    size_t i;

    /* Note: could be faster if we do machine-word-sized chunks. */
    for (i = 0; i < len; i++)
    {
        h ^= ((u8)buf[i]) & 0xFFUL;
        h *= PRIME_32;
    }

    return h;
}

/* Set out to the 64-bit truncated result of the unsigned multiplication of a *
 * b, where a and b are 64-bit little endian and out is 64-bit. */
/* Use long multiplication with 16-bit word size, so we can store the full
 * multiplication result in a word on a 32-bit machine, per Algorithm M from
 * Knuth Vol 2. Unlike Algorithm M, cut out all calculations for the upper
 * 64-bits of the result, as we know we won't use those; this makes the
 * calculation triangly. */
/* Note that this function does not work in place. out can't be one of the
 * input parameters, a or b. */
static void umul64(u16 out[], const u16 a[], const u16 b[])
{
    u32 k;
    u32 t;

    t = a[0] * b[0];
    out[0] = t;
    k = t >> 16;

    t = a[1] * b[0] + k;
    out[1] = t;
    k = t >> 16;

    t = a[2] * b[0] + k;
    out[2] = t;
    k = t >> 16;

    t = a[3] * b[0] + k;
    out[3] = t;

    t = a[0] * b[1] + out[1];
    out[1] = t;
    k = t >> 16;

    t = a[1] * b[1] + out[2] + k;
    out[2] = t;
    k = t >> 16;

    t = a[2] * b[1] + out[3] + k;
    out[3] = t;

    t = a[0] * b[2] + out[2];
    out[2] = t;
    k = t >> 16;

    t = a[1] * b[2] + out[3] + k;
    out[3] = t;

    t = a[0] * b[3] + out[3];
    out[3] = t;
}

/* FNV-1a algorithm description from
 * http://www.isthe.com/chongo/tech/comp/fnv/index.html#FNV-param */
static void hash_fnv1a_64(u32 hash[], const char *buf, size_t len)
{
    /* FNV-1a parameters for a 64-bit hash:
     * 64 bit FNV_prime = 1 << 40 + 1 << 8 + 0xB3
     *                  = 1099511628211 = 0x100000001B3
     * 64 bit offset_basis = 14695981039346656037 = 0xCBF29CE484222325
     */
    const u16 PRIME_64[4] = {0x01B3, 0x0000, 0x0100, 0x0000};
    const u16 OFFSET_64[4] = {0x2325, 0x8422, 0x9CE4, 0xCBF2};

    u16 h[4];
    u16 h_next[4];
    size_t i;

    h[0] = OFFSET_64[0];
    h[1] = OFFSET_64[1];
    h[2] = OFFSET_64[2];
    h[3] = OFFSET_64[3];

    /* Note: could be faster if we do machine-word-sized chunks. */
    for (i = 0; i < len; i++)
    {
        h[0] ^= ((u8)buf[i]) & 0xFFUL;

        /* Multiply h by PRIME_64 */
        umul64(h_next, h, PRIME_64);
        h[0] = h_next[0];
        h[1] = h_next[1];
        h[2] = h_next[2];
        h[3] = h_next[3];
        /* Note we are throwing away the upper 64-bits of the multiplication */
    }

    hash[0] = ((u32)h[1] << 16) | h[0];
    hash[1] = ((u32)h[3] << 16) | h[2];
}

unsigned long hash(const char *buf, size_t len)
{
    return hash_fnv1a_32(buf, len);
}

void hash64(u32 out[], const char *buf, size_t len)
{
    hash_fnv1a_64(out, buf, len);
}
