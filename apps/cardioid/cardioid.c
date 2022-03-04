/*
 *  cardioid.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2022-03-04.
 *  Copyright 2022. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "fps.h"
#include "guikit/font.h"
#include "guikit/graphics.h"
#include "guikit/panic.h"
#include "guikit/prandom.h"
#include "guikit/primrect.h"
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const int radius = SCREEN_HEIGHT / 2 - 16;

static float wrapf(float a, float max)
{
    /* Wrap a to between 0 and max inclusive */
    return a - floor(a / max) * max;
}

/* Remap the value from its original range to the new range. */
static float map(float val, float start, float end,
                 float newStart, float newEnd)
{
    return (val - start) / (end - start) * (newEnd - newStart) + newStart;
}

static void angleVector(float *x, float *y, float angle, float len)
{
    *x = len * cos(angle);
    *y = len * sin(angle);
}

static void getVector(float *x, float *y, float index, float total)
{
    float angle;

    angle = map(fmod(index, total), 0, total, 0.0f, 2.0f * M_PI);

    angleVector(x, y, angle + M_PI, radius);
}

static void drawHueShiftLine(int x0, int y0, int x1, int y1, float t)
{
    static float base = 0.0f;
    float hue;

    hue = wrapf(base + t * 256.0f, 256.0f);
    SetColorHSV(hue, 255, 255);
    DrawLine(x0, y0, x1, y1);

    base += 0.00001f;
    /* Keep base low magnitude so we don't end up not being able to increment.
     * We've seen, with float, base get stuck at 2048.0 with an addition of
     * 0.0001 having no effect. Wrapping at 256.0 should avoid any
     * discontinuity in hue. */
    base = wrapf(base, 256.0f);
}

/* Return distance between the point (x0, y0) and the line formed by points
 * (px1, py1) to (px2, py2) */
static float pointToLineDist(
    float x0, float y0,
    float px1, float py1, float px2, float py2)
{
    return fabs((px2 - px1) * (py1 - y0) - (px1 - x0) * (py2 - py1)) /
           sqrt(pow(px2 - px1, 2.0f) + pow(py2 - py1, 2.0f));
}

/* Returns non-zero if the provided cubic bezier curve is flat (very similar to
 * a straight line, within some epsilon). Refer to Figure 11.38 in Computer
 * Graphics: Principles and Practice, Second Edition, Equation 11.51. */
static int isFlat(const float px[4], const float py[4], float epsilon)
{
    float d2;
    float d3;

    /* Distance from first control point to line formed by endpoints. */
    d2 = pointToLineDist(px[1], py[1], px[0], py[0], px[3], py[3]);

    /* Distance from second control point to line formed by endpoints. */
    d3 = pointToLineDist(px[2], py[2], px[0], py[0], px[3], py[3]);

    return d2 < epsilon && d3 < epsilon;
}

/* Do one dimension of de Casteljau subdivision */
static void decasteljau(float l[4], float r[4], const float p[4])
{
    float h;

    /* Point H is half-way along the line between p[1]-p[2] (the two control
     * points). */
    h = (p[1] + p[2]) / 2.0f;

    l[0] = p[0];
    l[1] = (p[0] + p[1]) / 2.0f;
    l[2] = (l[1] + h) / 2.0f;

    r[3] = p[3];
    r[2] = (p[2] + p[3]) / 2.0f;
    r[1] = (h + r[2]) / 2.0f;

    l[3] = (l[2] + r[1]) / 2.0f;
    r[0] = l[3];
}

/* Divide the curve described by (px, py) into two curves, (lx, ly) and (rx,
 * ry). Use de Casteljau to find the new points, as described in Computer
 * Graphics: Principles and Practice, Second Edition, Equation 11.51. */
static void subdivBezier(float lx[4], float ly[4], float rx[4], float ry[4],
                         const float px[4], const float py[4])
{
    decasteljau(lx, rx, px);
    decasteljau(ly, ry, py);
}

static void drawBezierRecursive(const float px[4], const float py[4],
                                float epsilon, float t0, float t1)
{
    float lx[4];
    float ly[4];
    float rx[4];
    float ry[4];
    float t05;

    t05 = (t0 + t1) / 2.0f;

    if (isFlat(px, py, epsilon))
    {
        /* Draw a line between the curve's endpoints, because it's pretty much
         * flat. */
        /* t should be between 0 and 1 for colors */
        drawHueShiftLine(px[0], py[0], px[3], py[3], t05);
        return;
    }

    subdivBezier(lx, ly, rx, ry, px, py); /* Subdivide into left and right */

    drawBezierRecursive(lx, ly, epsilon, t0, t05); /* Left half */
    drawBezierRecursive(rx, ry, epsilon, t05, t1); /* Right half */
}

void drawBezier(const float px[4], const float py[4])
{
    static const float epsilon = 0.0250f;
    drawBezierRecursive(px, py, epsilon, 0.0f, 1.0f);
}


/* Times Table Cardioid drawn with Beziers (instead of simple lines) */
/* References
 * - https://thecodingtrain.com/CodingChallenges/133-times-tables-cardioid.html
 * - https://thecodingtrain.com/CodingChallenges/163-bezier.html
 */
void Cardioid(int frame)
{
    static const int total = 200;
    int i;
    float factor;
    float px[4];
    float py[4];

    factor = 2.0f + frame * 0.015f;

    SetColor(COLOR_BLACK);
    FillScreen();

    for (i = 0; i < total - 1; ++i)
    {
        getVector(&px[0], &py[0], i, total);
        getVector(&px[1], &py[1], i + 16, total);
        getVector(&px[2], &py[2], (i + 8) * factor, total);
        getVector(&px[3], &py[3], (i + 1) * factor, total);

        /* Would be cool to just move pen once, instead of manipulating control
         * points each time... */
        px[0] += SCREEN_WIDTH / 2;
        px[1] += SCREEN_WIDTH / 2;
        px[2] += SCREEN_WIDTH / 2;
        px[3] += SCREEN_WIDTH / 2;

        py[0] += SCREEN_HEIGHT / 2;
        py[1] += SCREEN_HEIGHT / 2;
        py[2] += SCREEN_HEIGHT / 2;
        py[3] += SCREEN_HEIGHT / 2;
        drawBezier(px, py);
    }

    SetColor(COLOR_WHITE);
    DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, radius);
}

int main()
{
    const struct Font *font;
    int ret;
    int i;
    int textHeight;

    ret = InitGraphicsBGR555("Cardioid");
    if (ret < 0)
    {
        panic("Couldn't init graphics\n");
    }

    font = GetFont("Orange");
    textHeight = 13;
    if (!font)
    {
        panic("Couldn't get font\n");;
    }

    printf("Drawing some curves...\n");
    InitFPSTimer();
    for(i = 0; i < 60 * 30; ++i)
    {
        Cardioid(i);

        DisplayFPS(font, textHeight);
        ShowGraphics();
    }
    SaveScreenShot("cardioid.bmp");
    FreeGraphics();

    printf("Goodbye.\n");

    return 0;
}
