/*
 *  fill.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-04-05.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/graphics.h"
#include "guikit/panic.h"
#include <stdio.h>

int main()
{
    int ret;
    int i;

    ret = InitGraphics();
    if (ret < 0)
    {
        panic("Couldn't init graphics\n");
    }

    printf("Hello world\n");

    /* Display a blue screen for about 2 seconds. */
    for(i = 0; i < 60 * 2; ++i)
    {
        FillScreen(COLOR_BLUE);
        ShowGraphics();
    }
    SaveScreenShot("fill.bmp");
    FreeGraphics();

    return 0;
}
