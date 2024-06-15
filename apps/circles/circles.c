/*
 *  circles.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2020-12-20.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/graphics.h"
#include "guikit/prandom.h"
#include "guikit/panic.h"
#include <stdio.h>

void RandomCircles(int num)
{
    int color;
    int x;
    int y;
    int r;
    int i;

    for (i = 0; i < num; ++i)
    {
        color = RandRange(0, NUM_COLORS - 1);
        SetColor(color);
        x = RandRange(0, SCREEN_WIDTH - 1);
        y = RandRange(0, SCREEN_HEIGHT - 1);
        r = RandRange(1, 100);
        if (RandRange(0, 1) == 0)
        {
            FillCircle(x, y, r);
        }
        else
        {
            DrawCircle(x, y, r);
        }

        if (i % 100 == 0)
        {
            ShowGraphics();
        }
    }
}

void TestDrawCircles(void)
{
    SetColor(COLOR_GREEN);
    DrawCircle(24, 44, 12);
    SetColor(COLOR_RED);
    FillCircle(44, 44, 12);
}

int main(void)
{
    int ret;

    ret = InitGraphics();
    if (ret < 0)
    {
        panic("Couldn't init graphics\n");
    }

    printf("Drawing 10000 circles...\n");
    RandomCircles(10000);
    ShowGraphics();
    SaveScreenShot("circles.bmp");
    FreeGraphics();

    printf("Goodbye.\n");

    return 0;
}
