/*
 *  primes.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2022-02-02.
 *  Copyright 2022. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "bitset.h"
#include "guikit/font.h"
#include "guikit/graphics.h"
#include "guikit/panic.h"
#include "guikit/prandom.h"
#include "guikit/primrect.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

enum
{
    /* Look for primes within the range from 0 to this number. This should be
     * enough for each pixel on the screen and then some. */
    NUMBERS = 1000000
};

/* We store 1 bit per number in our bit array. We only store odd numbers (evens
 * are composite). */
static unsigned char bitset[NUMBERS / 2 / 8 + 1];

static size_t maxPrime(void)
{
    /* We can stop looking for primes after we hit the square root of the
     * largest number being considered. */
    return sqrt(NUMBERS);
}

static void sieve(void)
{
    size_t maxPrimeBit;
    size_t maxBit;
    size_t factorBit;

    /* The location of the biggest possible prime in our bit array (which only
     * holds odd numbers). */
    maxPrimeBit = (maxPrime() + 1) / 2;
    maxBit = NUMBERS / 2;

    /* 2 is prime already. Start search from the first odd number, 3. Keep in
     * mind our bit array is only storing odd numbers, so 3 is at index 1. */
    factorBit = 1;

    while (factorBit <= maxPrimeBit)
    {
        size_t i;
        size_t factorNum;

        /* Find the first unset number in our bit array. This will be a prime.
         * */
        for (i = factorBit; i < maxBit; ++i)
        {
            if (!BitsetTestBit(bitset, i))
            {
                /* This factor hasn't been marked as composite. */
                break;
            }
        }

        factorNum = i * 2 + 1;
        factorBit = i + 1;

        /* Mark all multiples of this prime as composite. Start at the
         * prime*prime multiple, as any smaller multiples will have been marked
         * already from previous primes. */
        i = factorNum; /* Convert bit index to number */
        i *= i; /* Multiply to get next to the first multiple to consider */
        i /= 2; /* Convert number back to bit index */
        /* Increment by factorNum as that will take us to the next multiple in
         * our bit array. */
        for (; i < maxBit; i += factorNum)
        {
            BitsetSet(bitset, i);
        }
    }
}

static size_t countPrimes(void)
{
    /* The number 1 technically isn't prime, but it is at index 0 in our bit
     * array. We count it because 2 is prime, and 2 isn't stored in our bit
     * array. 1 takes the place of 2 in our count. */
    return BitsetCountClearBits(bitset, sizeof(bitset));
}

static unsigned char isPrime(int n)
{
    if (n == 2)
    {
        /* sieve() doesn't set 2 in the bit set, because it can't. The bitset
         * only has odd numbers in it to save some memory. */
        return 1;
    }

    if (n <= 1 || n % 2 == 0)
    {
        return 0;
    }

    return !BitsetTestBit(bitset, n / 2);
}

/* Ulam Prime Spiral */
/* References
 * - https://thecodingtrain.com/CodingChallenges/167-prime-spiral.html
 */
static void PrimeSpiral(int frame)
{
    enum {
        RIGHT = 0,
        UP,
        LEFT,
        DOWN,
        NUM_DIRECTIONS
    };
    int x;
    int y;
    int n;

    int steps;
    int stepsTodo;
    int direction;
    int turnsTaken;

    SetColor(COLOR_BLACK);
    FillScreen();

    SetColor(COLOR_WHITE);
    n = 1;
    x = SCREEN_WIDTH / 2;
    y = SCREEN_HEIGHT / 2;
    direction = RIGHT;
    steps = 0;
    stepsTodo = 1;
    turnsTaken = 0;

    while (n < frame << 10)
    {
        /* Do we need to turn? */
        if (steps == stepsTodo)
        {
            /* If so, turn. */
            direction += 1;
            direction &= NUM_DIRECTIONS - 1;

            steps = 0;
            ++turnsTaken;

            /* Every two turns, increase steps we go by one. */
            if (turnsTaken == 2)
            {
                turnsTaken = 0;
                stepsTodo += 1;
            }
        }

        if (isPrime(n))
        {
            struct Rect rect;
            rect.left = x;
            rect.top = y;
            rect.right = x;
            rect.bottom = y;

            SetColor(COLOR_LIGHT_BLUE);
            FillRect(&rect);
        }

        /* Move */
        switch (direction)
        {
        case RIGHT:
            x += 1;
            break;
        case UP:
            y -= 1;
            break;
        case LEFT:
            x -= 1;
            break;
        case DOWN:
            y += 1;
            break;
        }

        ++n;
        ++steps;
    }
}

int main()
{
    int ret;
    int i;

    ret = InitGraphics();
    if (ret < 0)
    {
        panic("Couldn't init graphics\n");
    }

    printf("Calculating primes...\n");
    sieve();
    printf("Found %lu primes between 1 and %d\n", countPrimes(), NUMBERS);

    printf("Drawing prime spiral...\n");
    for(i = 0; i < 60 * 10; ++i)
    {
        PrimeSpiral(i);
        ShowGraphics();
    }
    SaveScreenShot("primes.bmp");
    FreeGraphics();

    printf("Goodbye.\n");

    return 0;
}
