/*
 *  lines.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2020-12-13.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/graphics.h"
#include "guikit/panic.h"
#include <stdio.h>

void VectorsUp(int color, int x0, int y0, int xlen, int ylen)
{
    int x;
    int y;

    /* Lines from center to top of rectangle */
    x = x0 - xlen;
    y = y0 - ylen;
    for(; x < x0 + xlen; x++)
    {
        DrawLine(color, x0, y0, x, y);
        ShowGraphics();
    }
    /* Lines from center to right of rectangle */
    x = x0 + xlen - 1;
    y = y0 - ylen;
    for(; y < y0 + ylen; y++)
    {
        DrawLine(color, x0, y0, x, y);
        ShowGraphics();
    }
    /* Lines from center to bottom of rectangle */
    x = x0 + xlen - 1;
    y = y0 + ylen - 1;
    for(; x >= x0 - xlen; x--)
    {
        DrawLine(color, x0, y0, x, y);
        ShowGraphics();
    }
    /* Lines from center to left of rectangle */
    x = x0 - xlen;
    y = y0 + ylen - 1;
    for(; y >= y0 - ylen; y--)
    {
        DrawLine(color, x0, y0, x, y);
        ShowGraphics();
    }
}

void TestDrawLines(void)
{
    enum { X_MAX = 640, Y_MAX = 480 };

    VectorsUp(COLOR_BLUE, X_MAX / 4, Y_MAX / 4, X_MAX / 4, Y_MAX / 4);
    VectorsUp(COLOR_GREEN, X_MAX * 3 / 4, Y_MAX / 4, X_MAX / 4, Y_MAX / 4);
    VectorsUp(COLOR_LIGHT_BLUE, X_MAX / 4, Y_MAX * 3 / 4, X_MAX / 4, Y_MAX / 4);
    VectorsUp(COLOR_RED, X_MAX * 3 / 4, Y_MAX * 3 / 4, X_MAX / 4, Y_MAX / 4);
}

int main()
{
    int ret;

    ret = InitGraphics();
    if (ret < 0)
    {
        panic("Couldn't init graphics\n");
    }

    printf("Drawing loads of lines...\n");
    TestDrawLines();
    SaveScreenShot("lines.bmp");
    FreeGraphics();

    printf("Goodbye.\n");

    return 0;
}
