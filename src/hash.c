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
static u32 hash_fnv1a(const char *buf, size_t len)
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

unsigned long hash(const char *buf, size_t len)
{
    return hash_fnv1a(buf, len);
}
