/*
 *  waggle.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2022-02-20.
 *  Copyright 2022. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/graphics.h"
#include "guikit/prandom.h"
#include "guikit/primrect.h"
#include "guikit/panic.h"
#include <stdio.h>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DUAL_WAVE 0
#define CHONKY_LOOK 0

int xor256(unsigned x, unsigned y)
{
    return (x ^ y) % 256;
}

static int tex256(unsigned x, unsigned y)
{
    /* Sample an XOR texture. */
    return xor256(x, y);
}

static float wrapf(float a, float max)
{
    /* Wrap a to between 0 and max inclusive */
    return a - floor(a / max) * max;
}

static int wagglesamp(int x, int y, int t)
{
    int tex;

    float u;
    float v;

    int palShift; /* Palette shift */

    float f[2]; /* Frequency */
    float a[2]; /* Amplitude */
    float p[2]; /* Phase */
    float s[2]; /* Linear Scroll */

    palShift = 3;

    f[0] = 1.0f / (5.0f * M_PI);
    a[0] = 4.0f * M_PI;
    p[0] = 0.01f;
    s[0] = 1.0f;

    f[1] = 1.0f / (8.0f * M_PI);
    a[1] = 7.0f;
    p[1] = 0.2f;
    s[1] = 2.0f;

    u = x;
#if DUAL_WAVE
    if (y % 2 == 0)
    {
        u += a[0] * sin(f[0] * y + p[0] * t);
    }
    else
    {
        u -= a[0] * sin(f[0] * y + p[0] * t);
    }
#else
    u += a[0] * sin(f[0] * y + p[0] * t);
#endif
    u -= s[0] * t;
    u = wrapf(u, 256.0f);

    v = y;
    v += a[1] * sin(f[1] * y + p[1] * t);
    v -= s[1] * t;
    v = wrapf(v, 256.0f);

    tex = tex256(u, v);

    /* Palette shift */
    tex += t * palShift;
    tex %= 256;

    return tex;
}

void RainbowWaggle(int frame)
{
    enum {
#if CHONKY_LOOK
        /* Use 2x2 fat pixels for a chonky look. */
        w = 2,
        h = 2
#else
        /* Use 1x1 pixels for a finer appearance, even though using rectangle
         * drawing functions for 1x1 pixels is a bit silly. */
        w = 1,
        h = 1
#endif
    };
    unsigned int i;
    unsigned int j;
    struct Rect rect;
    NormalizedRect(&rect, &rect);

    rect.top = 0;
    rect.bottom = h;

    for (j = 0; j < SCREEN_HEIGHT; ++j)
    {
        rect.left = 0;
        rect.right = w;

        for (i = 0; i < SCREEN_WIDTH; ++i)
        {
            SetColorHSV(wagglesamp(i, j, frame), 255, 255);
            FillRect(&rect);
            rect.left += w;
            rect.right += w;
            NormalizedRect(&rect, &rect);
        }
        rect.top += h;
        rect.bottom += h;
    }

    ShowGraphics();
}

int main()
{
    int ret;
    int i;

    ret = InitGraphicsBGR555("Waggle");
    if (ret < 0)
    {
        panic("Couldn't init graphics\n");
    }

    printf("Waggling...\n");
    for (i = 0; i < 60 * 1; ++i)
    {
        RainbowWaggle(i);
    }
    SaveScreenShot("waggle.bmp");
    FreeGraphics();

    printf("Goodbye.\n");

    return 0;
}
