/*
 *  histgram.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-04-15.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/histgram.h"
#include "guikit/graphics.h"
#include "guikit/prandom.h"
#include "guikit/primrect.h"
#include "guikit/panic.h"
#include <stdio.h>

static void drawAxes(const struct Rect *r)
{
    int width;
    int height;

    width = r->right - r->left + 2; /* + 2 to include the vertical axis */
    height = r->bottom - r->top + 1;

    /* y-axis */
    SetColor(COLOR_BLUE);
    DrawVertLine(r->left - 1, r->top, height);

    /* x-axis */
    SetColor(COLOR_GREEN);
    DrawHorizLine(r->left - 1, r->bottom + 1, width);
}

void RandomHistograms(int num)
{
    int i;

    for (i = 0; i < num; ++i)
    {
        struct Histogram *h;
        struct Rect rect;
        int color;
        int numBuckets;
        int maxValue;
        int maxSize;
        int valuesToAdd;
        int j;
        int width;
        int fattness;

        color = RandRange(0, NUM_COLORS - 1);

        rect.left = RandRange(-10, SCREEN_WIDTH + 9);
        rect.top = RandRange(-10, SCREEN_HEIGHT + 9);
        rect.right = RandRange(0, SCREEN_WIDTH + 9);
        rect.bottom = RandRange(0, SCREEN_HEIGHT + 9);
        NormalizedRect(&rect, &rect);

        width = rect.right - rect.left;
        numBuckets = width - 1;

        /* Width zero histograms aren't interesting to see. Also, our
         * "pretty" histogram generator doesn't handle generating an
         * appropriate number of buckets when the width is 0. */
        numBuckets = numBuckets > 0 ? numBuckets : 1;

        /* Make some bars fat. */
        /* To be pretty, the number of buckets should be an integer dividend
         * of the rect width */
        fattness = RandRange(0, 10);
        for (j = 0; j < fattness; ++j)
        {
            numBuckets /= 2;
        }
        numBuckets = numBuckets > 0 ? numBuckets : 1;

        maxValue = RandRange(1, 1000000);
        maxSize = numBuckets * 2;

        h = HistogramAlloc(numBuckets);

        HistogramSetMaxValue(h, maxValue);

        valuesToAdd = RandRange(100, maxSize * 2);
#if 0
        printf("numBuckets %d\n", numBuckets);
        printf("maxValue %d\n", maxValue);
        printf("maxSize %d\n", maxSize / numBuckets);
        printf("valuesToAdd %d\n", valuesToAdd);
        printf("width %d\n", width);
#endif

        for (j = 0; j < valuesToAdd; ++j)
        {
            int val;

            val = RandRange(0, maxValue);

            HistogramPush(h, val);
        }

        SetColor(COLOR_WHITE);
        FillScreen();
        /*FillRect(COLOR_WHITE, &rect);*/
        drawAxes(&rect);
        DrawHistogram(h, color, &rect);

        HistogramFree(h);

        ShowGraphics();
    }
}

void RandomHistogram(int samples)
{
    struct Histogram *h;
    struct Rect rect;
    int numBuckets;
    int maxValue;
    int j;
    int width;

    rect.left = 20;
    rect.top = 20;
    rect.right = SCREEN_WIDTH - 1 - 20;
    rect.bottom = SCREEN_HEIGHT - 1 - 20;

    width = rect.right - rect.left + 1;
    numBuckets = width;
    maxValue = 2147483647;

    h = HistogramAlloc(numBuckets);

    HistogramSetMaxValue(h, maxValue);

    for (j = 0; j < samples; ++j)
    {
        int val;

        val = RandRange(0, maxValue);
        /*val = prandom() & 0x7FFFFFFF; *//* Cap to int max */

        HistogramPush(h, val);
    }

    SetColor(COLOR_WHITE);
    FillScreen();
    /*FillRect(COLOR_WHITE, &rect);*/
    /* XXX Looks height is not obeyed... Should we clip? or let histogram
     * outgrow its rect? Let it be ugly if the client asks for it? */
    drawAxes(&rect);
    DrawHistogram(h, COLOR_RED, &rect);

    HistogramFree(h);

    ShowGraphics();
}

void SampledHistogram(int samples)
{
    struct Histogram *u;
    struct Histogram *n;
    struct Rect rect;
    int i;
    int width;
    int numBuckets;
    int maxValue;
    int minSize;
    int maxSize;

    rect.left = 20;
    rect.top = 20;
    rect.right = SCREEN_WIDTH - 1 - 20;
    rect.bottom = SCREEN_HEIGHT - 1 - 20;

    width = rect.right - rect.left + 1;
    numBuckets = width;
    maxValue = 0x7FFFFFFF;

    u = HistogramAlloc(numBuckets);

    HistogramSetMaxValue(u, maxValue);
    HistogramSetMinValue(u, 0);

    for (i = 0; i < samples; ++i)
    {
        int val;

        val = prandom() & 0x7FFFFFFF; /* Cap to int max */

        HistogramPush(u, val);
    }

    /* Monte Carlo! Samping the samples to show sampling theorem works. */
    minSize = HistogramGetMinSize(u);
    maxSize = HistogramGetMaxSize(u);
    n = HistogramAlloc(maxSize - minSize);
    HistogramSetMaxValue(n, maxSize);
    HistogramSetMinValue(n, minSize);

    for (i = 0; i < samples; ++i)
    {
        size_t which;
        int value;

        which = RandRange(0, numBuckets - 1);
        value = HistogramGet(u, which);

        HistogramPush(n, value);
    }

    SetColor(COLOR_WHITE);
    FillScreen();
    /*FillRect(COLOR_WHITE, &rect);*/
    drawAxes(&rect);
    DrawHistogram(n, COLOR_RED, &rect);

    HistogramFree(u);
    HistogramFree(n);

    ShowGraphics();
}

int main()
{
    int ret;

    ret = InitGraphics();
    if (ret < 0)
    {
        panic("Couldn't init graphics\n");
    }

    printf("Drawing 10000 histograms...\n");

    SetColor(COLOR_WHITE);
    FillScreen();
    ShowGraphics();

    /*RandomHistograms(10000);*/
    /*RandomHistogram(1000000);*/
    SampledHistogram(10000);

    SaveScreenShot("histgram.bmp");
    FreeGraphics();

    printf("Goodbye.\n");

    return 0;
}
