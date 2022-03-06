/*
 *  bitset.h
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2022-02-02.
 *  Copyright 2022. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef BITSET_H
#define BITSET_H

#include <stddef.h>

/* Set the specified bit */
void BitsetSet(unsigned char *bitset, size_t bit);

/* Reset (or clear) the specified bit */
void BitsetReset(unsigned char *bitset, size_t bit);

/* Return how many bits are set. */
/* len of bitset in bytes */
size_t BitsetCountSetBits(const unsigned char *bitset, size_t len);

/* Return how many bits are clear. */
/* len of bitset in bytes */
size_t BitsetCountClearBits(const unsigned char *bitset, size_t len);

/* Return non-zero if the specified bit is set. */
unsigned char BitsetTestBit(const unsigned char *bitset, size_t bit);

/* Clear the bitset (reset all bits) */
/* len of bitset in bytes */
void BitsetClearBits(unsigned char *bitset, size_t len);

#endif
