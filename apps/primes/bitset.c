/*
 *  bitset.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2022-02-02.
 *  Copyright 2022. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "bitset.h"
#include <stddef.h>
#include <strings.h>

void BitsetSet(unsigned char *bitset, size_t bit)
{
    bitset[bit / 8] |= 1 << (bit % 8);
}

void BitsetReset(unsigned char *bitset, size_t bit)
{
    bitset[bit / 8] &= ~(1 << (bit % 8));
}

size_t BitsetCountSetBits(const unsigned char *bitset, size_t len)
{
    size_t i;
    size_t count;

    count = 0;

    for (i = 0; i < len * 8; ++i)
    {
        count += !!BitsetTestBit(bitset, i);
    }

    return count;
}

size_t BitsetCountClearBits(const unsigned char *bitset, size_t len)
{
    size_t i;
    size_t count;

    count = 0;

    for (i = 0; i < len * 8; ++i)
    {
        count += !BitsetTestBit(bitset, i);
    }

    return count;
}

unsigned char BitsetTestBit(const unsigned char *bitset, size_t bit)
{
    return bitset[bit / 8] & (1 << (bit % 8));
}

void BitsetClearBits(unsigned char *bitset, size_t len)
{
    bzero(bitset, len);
}
