/**
 *  @file graphics.h
 *  @brief Graphics
 *
 *  Patater GUI Kit
 *  Created by Jaeden Amero on 2020-12-12.
 *  Copyright 2020. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

struct Rect;

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

/* Drawing operations */
enum {
    OP_NONE,
    OP_SRC_INV,
    OP_PAT_AND,
    OP_PAT_NOT,
    OP_PAT_OR,
    OP_PAT_XOR,
    OP_SRC_INV_PAT_XOR,
    OP_SRC_INV_PAT_OR,
    OP_SRC_INV_PAT_AND,
    NUM_OP
};

int InitGraphics(void);
int InitGraphicsBGR555(const char *name);
void FreeGraphics(void);

/* Some graphics implementations render to a buffer and then copy to the screen
 * all at once. Some other (potentially flickery) implementations render
 * directly to the screen in real-time, and for these implementations this
 * function may not do too much. */
void ShowGraphics(void);

void SetColor(int color);
void SetColorRGB(unsigned char r, unsigned char g, unsigned char b);
void SetColorHSV(unsigned char h, unsigned char s, unsigned char v);
void SetBGColor(int color);
void SetBGColorRGB(unsigned char r, unsigned char g, unsigned char b);
void SetBGColorHSV(unsigned char h, unsigned char s, unsigned char v);

int SaveScreenShot(const char *path);

void FillScreen(void);

void DrawRect(const struct Rect *rect);
void FillRect(const struct Rect *rect);
void FillRectOp(const unsigned char *pattern, int op, const struct Rect *rect);

void DrawVertLine(int x1, int y1, int len);
void DrawHorizLine(int x1, int y1, int len);
void DrawDiagLine(int x1, int y1, int x2, int len);
void DrawLine(int x1, int y1, int x2, int y2);

void DrawCircle(int x0, int y0, int radius);
void FillCircle(int x0, int y0, int radius);

void DrawRoundRect(int x0, int y0, int radius, int width, int height);
void FillRoundRect(int x0, int y0, int radius, int width, int height);

int DrawBitmap(const struct Rect *dst, int span, const unsigned char *img,
               const unsigned char *mask);
int DrawColorBitmap(const struct Rect *dst, int span, const unsigned char *img,
                    const unsigned char *mask);

int Blit(const unsigned char *img, const struct Rect *dst,
         const struct Rect *src, int span);
int BlitOp(const unsigned char *img, int op, const struct Rect *dst0,
           const struct Rect *src0, int span);
int BlitWithMask(const unsigned char *img, const unsigned char *mask,
                 const struct Rect *dst, const struct Rect *src, int span);

#endif
