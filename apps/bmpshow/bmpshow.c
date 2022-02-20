/*
 *  bmpshow.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-04-15.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/graphics.h"
#include "guikit/bmp.h"
#include "guikit/prandom.h"
#include "guikit/primrect.h"
#include "guikit/panic.h"
#include <stdio.h>

static int isColor(short planes)
{
    return planes == 4;
}

void RandomColorBitmaps(int num, int span, const unsigned char *img,
                        const unsigned char *mask, int width, int height)
{
    struct Rect dst;
    int i;

    for (i = 0; i < num; ++i)
    {
        dst.left = RandRange(-width, SCREEN_WIDTH - 1);
        dst.top = RandRange(-height, SCREEN_HEIGHT - 1);
        dst.right = dst.left + width;
        dst.bottom = dst.top + height;

        DrawColorBitmap(&dst, span, img, mask);

        ShowGraphics();
    }
}

void RandomBitmaps(int num, int span, const unsigned char *img,
                   const unsigned char *mask, int width, int height)
{
    int color;
    struct Rect dst;
    int i;

    for (i = 0; i < num; ++i)
    {
        color = RandRange(0, NUM_COLORS - 2); /* Exclude white */
        SetColor(color);
        dst.left = RandRange(-width, SCREEN_WIDTH - 1);
        dst.top = RandRange(-height, SCREEN_HEIGHT - 1);
        dst.right = dst.left + width;
        dst.bottom = dst.top + height;

        DrawBitmap(&dst, span, img, mask);

        ShowGraphics();
    }
}

int main(int argc, const char *argv[])
{
    int ret;

    struct Rect imgRect;
    struct Rect maskRect;
    short imgPlanes;
    short maskPlanes;
    unsigned char *img;
    unsigned char *mask;
    const char *imgPath;
    const char *maskPath;
    int span;

    if (argc < 3)
    {
        /* Using defaults */
        imgPath = "icons/floppy.bmp";
        maskPath = "masks/floppy.bmp";
    }
    else
    {
        imgPath = argv[1];
        maskPath = argv[2];
    }

    ret = InitGraphics();
    if (ret < 0)
    {
        panic("Couldn't init graphics\n");
    }

    img = bmp_load(imgPath, &imgRect, &imgPlanes);
    if (!img)
    {
        panic("Couldn't load img from \"%s\"\n", imgPath);
    }

    mask = bmp_load(maskPath, &maskRect, &maskPlanes);
    if (!mask)
    {
        panic("Couldn't load mask from \"%s\"\n", maskPath);
    }

    printf("Drawing 10000 bitmaps...\n");
    SetColor(COLOR_WHITE);
    FillScreen();
    ShowGraphics();
    span = imgRect.right + 1;
    if (isColor(imgPlanes))
    {
        RandomColorBitmaps(10000, span, img, mask, imgRect.right, imgRect.bottom);
    }
    else
    {
        RandomBitmaps(10000, span, img, mask, imgRect.right, imgRect.bottom);
    }

    SaveScreenShot("bmpshow.bmp");

    FreeGraphics();

    printf("Goodbye.\n");

    return 0;
}
