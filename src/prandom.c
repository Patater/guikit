/*
 *  lfsr.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-05-03.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/ptypes.h"
#include "guikit/panic.h"
#include <stdio.h>

#if LFSR
#define BIT(x) (1 << (x))

/* Polynomial selected from Koopman's list of Maximal Length LFSR Feedback
 * Terms <https://users.ece.cmu.edu/~koopman/lfsr/32.txt> */
static const u32 polynomial = 0x80000412;

static u32 lfsr_next(u32 state, u32 poly)
{
    /* Chosen polynomial = 0x80000412, because doesn't have that many 1 bits in
     * it.*/
    /* x^31 + x^10 + x^4 + x^1 */
    /* 0x81000021 */
    /* x^31 + x^24 + x^5 + 1 */
    u32 bit_31;
    u32 bit_10;
    u32 bit_4;
    u32 bit_1;

    u32 bit_24;
    u32 bit_5;
    u32 bit_0;
    (void)poly;

    bit_31 = (state & BIT(31)) >> 31; /* 0x80000000 */
    bit_10 = (state & BIT(10)) >> 10; /* 0x00000400 */
    bit_4 = (state & BIT(4)) >> 4;    /* 0x00000010 */
    bit_1 = (state & BIT(1)) >> 1;    /* 0x00000002 */

    bit_24 = (state & BIT(24)) >> 24; /* 0x01000000 */
    bit_5 = (state & BIT(5)) >> 5;    /* 0x00000020 */
    bit_0 = (state & BIT(0)) >> 0;    /* 0x00000001 */

    printf("\tstate = %x\n", state);
#if 0
    printf("\tstate << 1 = %x\n", state << 1);
    printf("\t%x = %x ^ %x ^ %x ^ %x\n", bit_31 ^ bit_10 ^ bit_4 ^ bit_1,
           bit_31, bit_10, bit_4, bit_1);
#endif

    state <<= 1;
#if 0
    state |= bit_31 ^ bit_10 ^ bit_4 ^ bit_1; /* new "bit_0" */
/* for 24-bit 23, 22, 21, 16 */
#endif
    state |= bit_31 ^ bit_24 ^ bit_5 ^ bit_0; /* new "bit_0" */

    return state;
}
#endif

u32 xorshift32(u32 x)
{
    /* Choice of [a,b,c] triple selected from the choices listed on page 2 of
     * George Marsaglia's paper "Xorshift RNGs" */
    static const u32 a = 3;
    static const u32 b = 1;
    static const u32 c = 14;

    x ^= x << a;
    x ^= x >> b;
    x ^= x << c;

    return x;
}

u32 xorshift128(u32 *x)
{
    /* Choice of [a,b,c] triple selected from the choices listed on page 4 of
     * George Marsaglia's paper "Xorshift RNGs" */
    static const u32 a = 15;
    static const u32 b = 4;
    static const u32 c = 21;

    u32 tmpx;

    tmpx = x[0];
    x[0] = x[1];
    x[1] = x[2];
    x[2] = x[3];

    tmpx ^= tmpx << a;
    tmpx ^= tmpx >> b; /* Should this go left or right? Doesn't seem to change the
    distribution that much either way... */
    x[3] ^= x[3] >> c;
    x[3] ^= tmpx;

    return x[3];
}

u32 prandom(void)
{
    static u32 state = 0x80000000;
    static u32 state128[4] = {1234567890U, 2718281828U, 2953575118U,
                              3141592653U};
    u32 x;

    state = xorshift32(state);
    x = xorshift128(state128);

    return x;
}

i32 RandRange(i32 smallest, i32 biggest)
{
    /* We could use all the bits, 0xFFFFFFFF, but then we'd need to rethink the
     * range algorithm a bit since it assumes random is >0 and fits in an int
     * */
    static const i32 max_rand = 0x7FFFFFFFUL;
    i32 ri;

    if (smallest == biggest)
    {
        /* This check prevents integer overflow when we attempt to add 1 to
         * max_rand (after dividing it by 1) in the return statement.  */
        return smallest;
    }

    ri = prandom() >> 1;
    return ri / (max_rand / (biggest - smallest + 1) + 1) + smallest;
}
