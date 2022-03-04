/*
 *  fps.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2022-03-05.
 *  Copyright 2022. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "fps.h"
#include "guikit/font.h"
#include "guikit/graphics.h"
#include "guikit/snprintf.h"
#include <stdio.h>
#include <sys/time.h>

static struct timeval start;
static struct timeval end;

void InitFPSTimer(void)
{
    gettimeofday(&start, NULL);
}

static int getFPS(void)
{
    static unsigned int frames = 0;
    static unsigned int fps;

    ++frames;

    gettimeofday(&end, NULL);

    /* If a second has gone by */
    if (end.tv_sec - start.tv_sec >= 1)
    {
        fps = frames;
        frames = 0;

        /* Re-arm the timer */
        gettimeofday(&start, NULL);
    }

    return fps;
}

void DisplayFPS(const struct Font *font, int textHeight)
{
    static char text[80];

    snprintf(text, sizeof(text), "%d", getFPS());
    SetColorRGB(255, 255, 255);
    DrawString(font, text, 5, SCREEN_HEIGHT - textHeight - 5);
}
