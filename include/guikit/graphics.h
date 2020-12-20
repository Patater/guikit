/*
 *  graphics.h
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2020-12-12.
 *  Copyright 2020. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

/* VGA resolution (in Mode 0x12) */
enum {
    SCREEN_WIDTH = 640,
    SCREEN_HEIGHT = 480
};

/* Standard Macintosh palette */
enum {
    COLOR_BLACK = 0,
    COLOR_DARK_GRAY = 1,
    COLOR_GRAY = 2,
    COLOR_LIGHT_GRAY = 3,
    COLOR_LIGHT_BROWN = 4,
    COLOR_BROWN = 5,
    COLOR_DARK_GREEN = 6,
    COLOR_GREEN = 7,
    COLOR_LIGHT_BLUE = 8,
    COLOR_BLUE = 9,
    COLOR_PURPLE = 10,
    COLOR_PINK = 11,
    COLOR_RED = 12,
    COLOR_ORANGE = 13,
    COLOR_YELLOW = 14,
    COLOR_WHITE = 15,
    NUM_COLORS
};

int InitGraphics(void);
void FreeGraphics(void);

/* Some graphics implementations render to a buffer and then copy to the screen
 * all at once. Some other (potentially flickery) implementations render
 * directly to the screen in real-time, and for these implementations this
 * function may not do too much. */
void ShowGraphics(void);

void SetColor(int color);

void FillScreen(int color);

void DrawRect(int color, int x, int y, int width, int height);
void FillRect(int color, int x, int y, int width, int height);
void FillRectOp(int bg_color, int fg_color, const unsigned char *pattern,
                int op, int x, int y, int width, int height);

void DrawVertLine(int x1, int y1, int len);
void DrawHorizLine(int x1, int y1, int len);
void DrawDiagLine(int x1, int y1, int x2, int len);
void DrawLine(int color, int x1, int y1, int x2, int y2);

void DrawCircle(int color, int x0, int y0, int radius);
void FillCircle(int color, int x0, int y0, int radius);

#endif
