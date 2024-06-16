/*
 *  bezline.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2022-03-03.
 *  Copyright 2022. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/font.h"
#include "guikit/graphics.h"
#include "guikit/panic.h"
#include "guikit/prandom.h"
#include "guikit/primrect.h"
#include "guikit/snprintf.h"
#include <math.h>
#include <stdio.h>
#include <sys/time.h>

#define DRAW_CIRCLES 0

static float lerp(float a, float b, float t)
{
    /* Linear interpolation */
    return a + t * (b - a);
}

static float wrapf(float a, float max)
{
    /* Wrap a to between 0 and max inclusive */
    return a - floor(a / max) * max;
}

/* Return a point in x and y based on linear Bezier control points in px and
 * py, along path t. */
/* Bezier equation B(t) for linear Bezier curves from
 * https://en.wikipedia.org/wiki/B%C3%A9zier_curve */
static void linearBezier(float *x, float *y, float px[2], float py[2], float t)
{
    float a[2];

    /* Compute a weights vector we can multiply the control points by to follow
     * the path. */
    a[0] = (1.0f - t);
    a[1] = t;

    *x = 0.0f;
    *x += a[0] * px[0];
    *x += a[1] * px[1];

    /* Compute the dot product of the weights vector with the y control points.
     * */
    *y = 0.0f;
    *y += a[0] * py[0];
    *y += a[1] * py[1];
}

/* Draw some "string art" lines to make one imagine a bezier curve.
 * References:
 *  - https://thecodingtrain.com/CodingChallenges/163-bezier.html
 */
void BezLines(int frame)
{
    float t;
    const float delta = 0.05f;
    float px[4];
    float py[4];
    float x[4];
    float y[4];

    /* Initialize end points */
    px[0] = 10.0f;
    py[0] = SCREEN_HEIGHT / 2.0f;

    px[3] = SCREEN_WIDTH - 10.0f;
    py[3] = SCREEN_HEIGHT / 2.0f;

    /* Initialize control points */
    px[1] = SCREEN_WIDTH / 8.0f;
    py[1] = SCREEN_HEIGHT / 8.0f * (1.0f - frame / 460.0f);

    px[2] = SCREEN_WIDTH / 8.0f * 7.0f;
    py[2] = SCREEN_HEIGHT / 8.0f * (7.0f + frame / 460.0f);

    /* Clear the screen so we start with a fresh canvas (no smudgies). */
    SetColor(COLOR_BLACK);
    FillScreen();

    for (t = 0.0f; t <= 1.00001f; t += delta)
    {
        float u[2];
        float v[2];
#if DRAW_CIRCLES
        float a;
        float b;
#endif

        /* Equivalent to one quadradic bezier */
        linearBezier(&x[0], &y[0], &px[0], &py[0], t);
        linearBezier(&x[1], &y[1], &px[1], &py[1], t);
        u[0] = lerp(x[0], x[1], t);
        v[0] = lerp(y[0], y[1], t);

        /* Equivalent to another quadradic bezier */
        linearBezier(&x[2], &y[2], &px[1], &py[1], t);
        linearBezier(&x[3], &y[3], &px[2], &py[2], t);
        u[1] = lerp(x[2], x[3], t);
        v[1] = lerp(y[2], y[3], t);

        /* Equivalent to a cubic bezier */
#if DRAW_CIRCLES
        a = lerp(u[0], u[1], t);
        b = lerp(v[0], v[1], t);
#endif

        /* Draw control points */
        SetColor(COLOR_WHITE);
        FillCircle(px[1], py[1], 8);
        FillCircle(px[2], py[2], 8);

        /* Draw the curves */
        SetColorHSV(wrapf(t * 256.0f, 256.0f), 255, 255);
#if DRAW_CIRCLES
        FillCircle(a, b, 8);
#endif
        DrawLine(x[0], y[0], x[1], y[1]);
        DrawLine(x[2], y[2], x[3], y[3]);
        DrawLine(u[0], v[0], u[1], v[1]);
    }

    ShowGraphics();
}

int main(void)
{
    int ret;
    int i;

    ret = InitGraphicsBGR555("Bezlines");
    if (ret < 0)
    {
        panic("Couldn't init graphics\n");
    }

    printf("Drawing some lines...\n");
    for(i = 0; i < 60 * 5; ++i)
    {
        BezLines(i);
    }
    SaveScreenShot("bezline.bmp");
    FreeGraphics();

    printf("Goodbye.\n");

    return 0;
}
