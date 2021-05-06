/*
 *  histgram.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-05-06.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/histgram.h"

#include "guikit/graphics.h"
#include "guikit/pmemory.h"
#include "guikit/primrect.h"
#include "guikit/array.h"
#include "guikit/panic.h"
#include <limits.h>

struct Histogram
{
    int maxValue; /* Most a value can be (x max). Largest value displayed. */
    int minValue; /* Least a value can be (x max). Smallest value displayed. */
    size_t numBuckets; /* How many different buckets */
    int *buckets;
};

struct Histogram *HistogramAlloc(size_t numBuckets)
{
    struct Histogram *h;
    size_t i;

    if (numBuckets > UINT_MAX / sizeof(*h->buckets))
    {
        panic("Too many buckets (%u > %u)\n",
              numBuckets, UINT_MAX / sizeof(*h->buckets));
    }

    h = pmalloc(sizeof(*h));

    h->numBuckets = numBuckets;
    h->maxValue = 200;
    h->minValue = 0;
    h->buckets = NULL;

    array_grow(h->buckets, h->numBuckets);
    for (i = 0; i < h->numBuckets; ++i)
    {
        array_push(h->buckets, 0);
    }

    return h;
}

void HistogramFree(struct Histogram *h)
{
    if (h)
    {
        array_free(h->buckets);
    }

    pfree(h);
}

void HistogramSetMaxValue(struct Histogram *h, int maxValue)
{
    h->maxValue = maxValue;
}

void HistogramSetMinValue(struct Histogram *h, int minValue)
{
    h->minValue = minValue;
}

static int arrayMax(int *a, int len)
{
    int i;
    int biggest = 0;
    for (i = 0; i < len; ++i)
    {
        biggest = a[i] > biggest ? a[i] : biggest;
    }

    return biggest;
}

int HistogramGetMaxSize(const struct Histogram *h)
{
    return arrayMax(h->buckets, array_len(h->buckets));
}

static int arrayMin(int *a, int len)
{
    int i;
    int smallest = INT_MAX;
    for (i = 0; i < len; ++i)
    {
        smallest = a[i] < smallest ? a[i] : smallest;
    }

    return smallest;
}

int HistogramGetMinSize(const struct Histogram *h)
{
    return arrayMin(h->buckets, array_len(h->buckets));
}

void HistogramPush(struct Histogram *h, int value)
{
    size_t which;
    size_t range;

    range = h->maxValue;
    range -= h->minValue;
    ++range;

    which = (value - h->minValue) * h->numBuckets / range;

    if (which >= array_cap(h->buckets))
    {
        panic("Value (%d/%u [%u - %u]) leads to too big which (%d) for "
              "buckets (%d/%d)\n",
              value, range, h->maxValue, h->minValue, which, h->numBuckets,
              array_cap(h->buckets));
    }
    #if 0
    printf("Value (%d/%lu [%u - %u]) to which (%lu) for buckets (%lu/%lu)\n",
           value, range, h->maxValue, h->minValue, which, h->numBuckets,
           array_cap(h->buckets));
    #endif

    ++h->buckets[which];
}

int HistogramGet(const struct Histogram *h, size_t which)
{
    return h->buckets[which];
}

static void drawBucket(int bval, int bwidth, int maxSize, int x, int y,
                       int height)
{
    struct Rect bar;
    int bheight;

    if (maxSize == 0)
    {
        panic("Would divide by zero\n");
    }

    bheight = height * bval / maxSize;
#if 0
    printf("bheight %d (%d * %d / %d)\n", bheight, height, bval, maxSize);
#endif

    bar.left = x;
    bar.right = bar.left + bwidth - 1;
    bar.bottom = y;
    bar.top = y - bheight + 1;
#if 0
    printf("l %d  r %d  b %d  t %d\n", bar.left, bar.right, bar.bottom,
           bar.top);
#endif

    FillRect(COLOR_RED, &bar);
}

static void drawBuckets(const struct Histogram *h, const struct Rect *rect,
                        int width, int height)
{
    size_t i;
    int x;
    int y;
    int bwidth;
    int maxSize;

    if (h->numBuckets == 0)
    {
        return;
    }

    x = rect->left;
    y = rect->bottom;
    bwidth = width / h->numBuckets;
    maxSize = HistogramGetMaxSize(h);

    for (i = 0; i < h->numBuckets; ++i)
    {
        /* XXX I need to accumlate an error term to add to the bar width
         * sometimes, so we end up at exact desired total width... but that's
         * ugly... */
        if (h->buckets[i] > maxSize)
        {
            panic("Too big bucket. Max func broken\n");
        }
        drawBucket(h->buckets[i], bwidth, maxSize, x, y, height);
        x += bwidth;
    }
}

void DrawHistogram(const struct Histogram *h, const struct Rect *rect)
{
    int width;
    int height;

    width = rect->right - rect->left + 1;
    height = rect->bottom - rect->top + 1;

    drawBuckets(h, rect, width, height);
}
