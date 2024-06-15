/*
 *  rect.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-04-05.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/graphics.h"
#include "guikit/prandom.h"
#include "guikit/primrect.h"
#include "guikit/panic.h"
#include <stdio.h>

void RandomRects(int num)
{
    int color;
    int i;

    for (i = 0; i < num; ++i)
    {
        struct Rect rect;
        color = RandRange(0, NUM_COLORS - 1);
        rect.left = RandRange(-10, SCREEN_WIDTH + 9);
        rect.top = RandRange(-10, SCREEN_HEIGHT + 9);
        rect.right = RandRange(-10, SCREEN_WIDTH + 9);
        rect.bottom = RandRange(-10, SCREEN_HEIGHT + 9);
        NormalizedRect(&rect, &rect);
        SetColor(color);
        if (RandRange(0, 1) == 0)
        {
            FillRect(&rect);
        }
        else
        {
            DrawRect(&rect);
        }

        if (i % 100 == 0)
        {
            ShowGraphics();
        }
    }
}

int main(void)
{
    int ret;

    ret = InitGraphics();
    if (ret < 0)
    {
        panic("Couldn't init graphics\n");
    }

    printf("Drawing 10000 rectangles...\n");
    RandomRects(10000);
    ShowGraphics();
    SaveScreenShot("rect.bmp");
    FreeGraphics();

    printf("Goodbye.\n");

    return 0;
}
