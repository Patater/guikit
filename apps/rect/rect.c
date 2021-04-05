/*
 *  rect.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-04-05.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/graphics.h"
#include "guikit/prandom.h"
#include "guikit/panic.h"
#include <stdio.h>

void RandomRects(int num)
{
    int color;
    int x;
    int y;
    int w;
    int h;
    int i;

    for (i = 0; i < num; ++i)
    {
        color = RandRange(0, NUM_COLORS - 1);
        x = RandRange(-10, SCREEN_WIDTH + 9);
        y = RandRange(-10, SCREEN_HEIGHT + 9);
        w = RandRange(-10, SCREEN_WIDTH + 9);
        h = RandRange(-10, SCREEN_HEIGHT + 9);
        if (RandRange(0, 1) == 0)
        {
            FillRect(color, x, y, w, h);
        }
        else
        {
            DrawRect(color, x, y, w, h);
        }
        ShowGraphics();
    }
}

int main()
{
    int ret;

    ret = InitGraphics();
    if (ret < 0)
    {
        panic("Couldn't init graphics\n");
    }

    printf("Drawing 10000 rectangles...\n");
    RandomRects(10000);
    FreeGraphics();

    printf("Goodbye.\n");

    return 0;
}
