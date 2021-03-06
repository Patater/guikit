/*
 *  graphics.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2020-12-12.
 *  Copyright 2020. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/graphics.h"
#include "guikit/primrect.h"
#include <limits.h>

#include "guikit/ansidos.h"
#include <pc.h>
#include <dos.h>
#include <go32.h>
#include <sys/farptr.h>

enum {
    VGA_VIDEO_SEGMENT = 0xA000
};

enum {
    GC_INDEX = 0x3CE,
    GC_DATA = 0x3CF
};

enum {
    GC_SET_RESET = 0,
    GC_ENABLE_SET_RESET = 1,
    GC_COLOR_COMPARE = 2,
    GC_DATA_ROTATE = 3,
    GC_READ_MAP_SELECT = 4,
    GC_GRAPHICS_MODE = 5,
    GC_MISCELLANEOUS = 6,
    GC_COLOR_DONT_CARE = 7,
    GC_BIT_MASK = 8,
    GC_NUM_REG
};

enum {
    SEQ_INDEX = 0x3C4
};

enum {
    SEQ_RESET = 0,
    SEQ_CLOCKING_MODE = 1,
    SEQ_MAP_MASK = 2,
    SEQ_CHARACTER_MAP_SELECT = 3,
    SEQ_MEMORY_MODE = 4,
    SEQ_NUM_REG
};

enum {
    DAC_PEL_MASK = 0x3C6,
    DAC_READ_ADDR = 0x3C7,
    DAC_WRITE_ADDR = 0x3C8,
    DAC_DATA = 0x3C9,
    DAC_NUM_REG
};

/* VGA DAC pallete (6-bit) uses 0x3F as max value. */
const unsigned char mac_pal[] = {
    0x00, 0x00, 0x00, /* Black */
    0x08, 0x08, 0x08, /* Dark Gray */
    0x20, 0x20, 0x20, /* Gray */
    0x30, 0x30, 0x30, /* Light Gray */
    0x24, 0x1C, 0x0E, /* Light Brown */
    0x15, 0x0B, 0x01, /* Brown */
    0x00, 0x19, 0x04, /* Dark Green */
    0x07, 0x2D, 0x05, /* Green */
    0x00, 0x2A, 0x3A, /* Light Blue */
    0x00, 0x00, 0x35, /* Blue */
    0x11, 0x00, 0x29, /* Purple */
    0x3C, 0x02, 0x21, /* Pink */
    0x37, 0x02, 0x01, /* Red */
    0x3F, 0x19, 0x00, /* Orange */
    0x3F, 0x3C, 0x01, /* Yellow */
    0x3F, 0x3F, 0x3F  /* White */
};

static struct Rect clip = {
    0,
    0,
    SCREEN_WIDTH - 1,
    SCREEN_HEIGHT - 1
};

/* An 8x8 1-bit bitmap, packed 1bpp */
static const unsigned char *currentPattern;

const unsigned char blackPattern[] = {0, 0, 0, 0, 0, 0, 0, 0};

static int currentMode = -1;

static unsigned long linear_ptr(unsigned long segment, unsigned long offset)
{
    return segment * 16 + offset;
}

static void set_gc(int index, int setting)
{
    outportw(GC_INDEX, (setting << 8) | index);
}

static unsigned char get_gc(int index)
{
    outportb(GC_INDEX, index);
    return inportb(GC_DATA);
}

static void set_seq(int index, int setting)
{
    outportw(SEQ_INDEX, (setting << 8) | index);
}

static int set_gfx_mode(int mode)
{
    union REGS r;
    r.x.ax = mode;
    int86(0x10, &r, &r);

    return 0;
}

static void set_palette(const unsigned char *palette, size_t bytes)
{
    /* Set palette RAM to passthrough mode. We want to use the DAC directly,
     * going from 16 color choices directly to 6-bit VGA color. */
    union REGS r;
    /*r.x.ax = 0x1002;*/ /* Probably faster to use int 10h, AH=10h, AL=2 */
    r.x.ax = 0x1000;
    r.x.bx = 0x0000;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0101;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0202;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0303;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0404;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0505;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0606;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0707;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0808;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0909;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0A0A;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0B0B;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0C0C;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0D0D;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0E0E;
    int86(0x10, &r, &r);
    r.x.ax = 0x1000;
    r.x.bx = 0x0F0F;
    int86(0x10, &r, &r);

    outportb(DAC_WRITE_ADDR, 0);

    while (bytes--)
    {
        outportb(DAC_DATA, *palette++);
    }
}

static void drawVertLine(int x, int y, int len)
{
    unsigned long dest = VGA_VIDEO_SEGMENT;
    unsigned char line;
    unsigned short col_in_byte;
    struct Rect rect;
    int ret;
    int i;

    /* Adjust clipping */
    InitRect(&rect, x, y, 1, len);
    ret = ClipRect(&rect, &clip);
    if (ret == CLIP_REJECTED)
    {
        return;
    }

    /* Calculate start byte. */
    /* 640 lines. 80 bytes each line. */
    dest = linear_ptr(VGA_VIDEO_SEGMENT, rect.top * 80 + rect.left / 8);
    col_in_byte = rect.left % 8;

    /* Calculate mask for line segments. */
    line = 0x80U >> col_in_byte;

    for (i = rect.top; i <= rect.bottom; ++i)
    {
        _farpeekb(_dos_ds, dest); /* Load latches */
        _farpokeb(_dos_ds, dest, line);
        dest += 80;
    }
}

static void drawHorizLine(int x, int y, int len)
{
    unsigned long dest = VGA_VIDEO_SEGMENT;
    unsigned short col_in_byte;
    unsigned char mask;
    int pixels;
    struct Rect rect;
    int ret;

    /*
     * Adjust line to clipping region.
     */
    InitRect(&rect, x, y, len, 1);
    ret = ClipRect(&rect, &clip);
    if (ret == CLIP_REJECTED)
    {
        return;
    }

    /* Calculate start byte. */
    /* 640 lines. 80 bytes each line. */
    dest = linear_ptr(VGA_VIDEO_SEGMENT, rect.top * 80 + rect.left / 8);
    col_in_byte = rect.left % 8; /* keep only the column in-byte address */

    mask = 0;
    pixels = rect.right - rect.left + 1;
    while (pixels--)
    {
        mask |= 0x80U >> col_in_byte;

        if ((col_in_byte == 7) || (pixels == 0))
        {
            /* We have collected a full byte. Write it. */
            /*set_gc(GC_BIT_MASK, mask);*/ /* Only needed if in mode 0 */
            _farpeekb(_dos_ds, dest); /* Load latches */
            _farpokeb(_dos_ds, dest++, mask);
            mask = 0;
            col_in_byte = 0;
            continue;
        }

        ++col_in_byte;
    }
}

static unsigned char rotate_right(unsigned char n)
{
    unsigned char bit;
    bit = (n & 1) << (CHAR_BIT * sizeof(n) - 1);
    return bit | n >> 1;
}

static unsigned char rotate_left(unsigned char n)
{
    unsigned char bit;
    bit = n >> (CHAR_BIT * sizeof(n) - 1);
    return n << 1 | bit;
}

static void drawDiagLine(int x1, int y1, int x2, int len)
{
    unsigned long dest = VGA_VIDEO_SEGMENT;
    unsigned char line;
    unsigned char orig_line;
    unsigned short col_in_byte;
    int i;
    int x_start;
    int x_end;
    int y_start;
    int y_end;
    struct Rect rect;
    int leftToRight;
    int ret;

    /* Right to left or left to right */
    /* Assumes top to bottom */
    y_start = y1;
    y_end = y1 + len - 1;

    if (x1 < x2)
    {
        x_start = x1;
        x_end = x2;
        leftToRight = 1;
    }
    else
    {
        x_start = x2;
        x_end = x1;
        leftToRight = 0;
    }

    /* Adjust clipping */
    rect.left = x_start;
    rect.right = x_end;
    rect.top = y_start;
    rect.bottom = y_end;
    ret = ClipRect(&rect, &clip);
    if (ret == CLIP_REJECTED)
    {
        return;
    }

    /* Adjust line shape based on clipping */
    if (leftToRight)
    {
        rect.left += rect.top - y_start; /* new_val - orig_val */
        rect.right -= y_end - rect.bottom;
        rect.top += rect.left - x_start;
        rect.bottom -= x_end - rect.right;
    }
    else
    {
        rect.right -= rect.top - y_start; /* new_val - orig_val */
        rect.left += y_end - rect.bottom;
        rect.bottom -= rect.left - x_start;
        rect.top += x_end - rect.right;
    }

    /* Like vertical line, but rotating the one single present
     * bit with each change in y */
    if (leftToRight)
    {
        /* Left-to-right */

        /* Calculate start byte. */
        /* 640 lines. 80 bytes each line. */
        dest = linear_ptr(VGA_VIDEO_SEGMENT, rect.top * 80 + rect.left / 8);
        col_in_byte = rect.left % 8;

        /* Calculate mask for line segments. */
        orig_line = 0x80U >> col_in_byte;
        line = orig_line;

        for (i = rect.top; i <= rect.bottom; ++i)
        {
            _farpeekb(_dos_ds, dest); /* Load latches */
            _farpokeb(_dos_ds, dest, line);
            dest += 80;
            if (line & 0x01)
            {
                ++dest;
            }
            line = rotate_right(line);
        }
    }
    else
    {
        /* Calculate start byte. */
        /* 640 lines. 80 bytes each line. */
        dest = linear_ptr(VGA_VIDEO_SEGMENT, rect.top * 80 + rect.right / 8);
        col_in_byte = rect.right % 8;

        /* Calculate mask for line segments. */
        orig_line = 0x80U >> col_in_byte;
        line = orig_line;

        /* Right-to-left */
        for (i = rect.top; i <= rect.bottom; ++i)
        {
            _farpeekb(_dos_ds, dest); /* Load latches */
            _farpokeb(_dos_ds, dest, line);
            dest += 80;
            if (line & 0x80)
            {
                --dest;
            }
            line = rotate_left(line);
        }
    }
}

static void setMode3Color(int color)
{
    /* Set Graphics Controller into write mode 3 */
    currentMode = 3;
    set_gc(GC_GRAPHICS_MODE, get_gc(GC_GRAPHICS_MODE) | 3);

    /* Enable all planes */
    set_seq(SEQ_MAP_MASK, 0xF);

    /* Set color in set/reset reg */
    set_gc(GC_ENABLE_SET_RESET, 0x0FU);
    set_gc(GC_SET_RESET, color);

    /* Set default bit mask */
    set_gc(GC_BIT_MASK, 0xFF);
}


void SetColor(int color)
{
    /* Assume we are in Mode 3 for now. */
    setMode3Color(color);
}

int InitGraphics()
{
    /* 640x480 16-color */
    set_gfx_mode(0x12);

    set_palette(mac_pal, sizeof(mac_pal));

    currentPattern = blackPattern;

    /* Reset the data rotate and bit mask registers. */
    /* XXX Maybe not needed after mode set. */
    set_gc(GC_DATA_ROTATE, 0);
    set_gc(GC_BIT_MASK, 0xFF);

    SetColor(COLOR_BLACK);
    FillScreen();

    return 0;
}

void ShowGraphics()
{
    return;
}

void FillScreen(int color)
{
    unsigned long dest;
    int i;

    /* Set write mode 0 */
    currentMode = 0;
    set_gc(GC_GRAPHICS_MODE, get_gc(GC_GRAPHICS_MODE) & 0xFC);

    /* Enable all planes */
    set_seq(SEQ_MAP_MASK, 0xF);

    /* Pick the color */
    set_gc(GC_SET_RESET, color);
    set_gc(GC_ENABLE_SET_RESET, 0x0FU);

    set_gc(GC_BIT_MASK, 0xFF);

    dest = linear_ptr(VGA_VIDEO_SEGMENT, 0);

    /* Fill all 64 KiB, DWORD at a time */
    for (i = 0; i < (64 * 1024)/4; ++i)
    {
        _farpokel(_dos_ds, dest, 0xFFFFFFFFU);
        dest += 4;
    }
}

void FreeGraphics(void)
{
    /* Reset the VGA hardware back to normal. */
    set_gc(GC_DATA_ROTATE, 0);
    set_gc(GC_ENABLE_SET_RESET, 0);
    set_gc(GC_BIT_MASK, 0xFF);

    /* Return to text mode. */
    set_gfx_mode(0x03);
}

void DrawRect(int color, const struct Rect *rect)
{
    int x1;
    int y1;
    int width;
    int height;

    x1 = rect->left;
    y1 = rect->top;
    width = rect->right + 1 - rect->left;
    height = rect->bottom + 1 - rect->top;

    setMode3Color(color);

    drawVertLine(x1, y1, height); /* Left */
    drawHorizLine(x1+1, y1, width-1); /* Top */
    drawVertLine(x1+width-1, y1+1, height-1); /* Right */
    drawHorizLine(x1+1, y1+height-1, width - 2); /* Bottom */
}

void FillRect(int color, const struct Rect *rect)
{
    int y;
    int width;

    width = rect->right - rect->left + 1;

    setMode3Color(color);

    /* Fill in everywhere */
    /* TODO do this quicker by doing this inline, having one loop instead of
     * two. Handle the rectangle left and right edge masking here, too. */
    for (y = rect->top; y <= rect->bottom; ++y)
    {
        drawHorizLine(rect->left, y, width);
    }
}

void FillRectOp(int bg_color, int fg_color, const unsigned char *pattern,
                int op, const struct Rect *rect)
{
    /* TODO */
    (void) bg_color;
    (void) pattern;
    (void) op;
    FillRect(fg_color, rect);
}

void DrawVertLine(int x1, int y1, int len)
{
    drawVertLine(x1, y1, len);
}

/* Test list */
/* To verify test, set port window in vga, and read from vram. take 4 reads
 * (one for each plane) to verify. need to write known image ahead of
 * time to verify background not modified at any position in any plane. */
/*
    - Draw horizontal line length 8 at 0 0 (full width of byte, byte aligned)
    - Draw horizontal line length 8 at 1 0 (last 7 bits first byte, first bit
      second byte)
    - Draw horizontal line length 7 at 1 0 (last 7 bits first byte)
    - Draw horizontal line length 8 at 7 0 (last 1 bit first byte, first 7 bits
      second byte)
    - Draw horizontal line length 9 at 0 0 (full width first, 1 bit second)
    - Draw horizontal line length 16 at 0 0 (full width first, second)
    - Draw horizontal line length 24 at 0 0 (full width first, second, third)
    - Draw horizontal line length 17 at 0 0 (full width first, second, 1 bit
      third)
    - Draw horizontal line length 16 at 1 0 (last 7 bits first, full second, 1 bit
      third)
*/
void DrawHorizLine(int x1, int y1, int len)
{
    drawHorizLine(x1, y1, len);
}

void DrawDiagLine(int x1, int y1, int x2, int len)
{
    drawDiagLine(x1, y1, x2, len);
}

/* XXX We are doing run-sliced bressenham. For small lines, standard bressenham
 * should be faster, (but only if we can figure out how to write more than 1
 * pixel at a time from standard bressenham). */
/* Provide standard bressenham here, too, so we can do short lines quickly,
 * without a divide or extra setup. */
void DrawLine(int color, int x1, int y1, int x2, int y2)
{
    int AdjUp; /* Error term adjust up on each advance*/
    int AdjDown; /* Error term adjust down when error term turns over*/
    int XAdvance; /* 1 or -1, for direction in which x advances*/
    int YDelta;
    int XDelta;
    int min_run_len;
    int final_run_len;
    int run_len; /* aka step */
    int errorAdj;

    setMode3Color(color);

    /*printf("(%03d,%03d)->(%03d,%03d)\r", x1, y1, x2, y2);*/

    /* We'll draw top to bottom, to reduce the number of cases we have to
     * handle, and to make lines between the same endpoints always draw the
     * same pixels. */
    /* TODO remove this, as we'll like patterned lines to work well without
     * having to reverse the pattern. */
    if (y1 > y2)
    {
        int tmp;

        /* Swap y1 and y2 */
        tmp = y1;
        y1 = y2;
        y2 = tmp;
        /* Swap x1 and x2 */
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    /* Now the line is top to bottom */

    /* Figure out how far we're going vertically (guaranteed to be positive) */
    YDelta = y2 - y1;

    /* Figure out whether we're going left or right, and how far we're going
     * horizontally. In the process, special-case vertical lines, for speed and
     * to avoid nasty boundary conditions and division by 0 */
    XDelta = x2 - x1;
    /* Do we have a vertical line? */
    if (XDelta == 0)
    {
        /* Yes, so use vertical line drawing */
        drawVertLine(x1, y1, YDelta+1);
        return;
    }
    else if (XDelta < 0)
    {
        /* Right to left */
        XAdvance = -1;
        XDelta = -XDelta; /* |XDelta| -- Make XDelta positive */
    }
    else
    {
        /* Left to right */
        XAdvance = 1;
    }

    /* Special-case horizontal lines */
    if (YDelta == 0)
    {
        if (XAdvance < 0)
        {
            /* Right-to-left */
            drawHorizLine(x2, y1, XDelta+1);
            return;
        }
        else
        {
            /* Left-to-right */
            drawHorizLine(x1, y1, XDelta+1);
            return;
        }
    }
    /* Special-case diagonal lines */
    else if (YDelta == XDelta)
    {
        drawDiagLine(x1, y1, x2, XDelta+1);
        return;
    }

    /* Determine whether the line is X or Y major, and handle accordingly. */
    if (XDelta > YDelta)
    {
        /* X-major */
        /* More horizontal than vertical */

        min_run_len = XDelta / YDelta;
        errorAdj = XDelta % YDelta;
        AdjUp = errorAdj + errorAdj;
        AdjDown = YDelta + YDelta;

        /* Initial error term; reflects an initial step of 0.5 along the Y
         * axis */
        errorAdj -= AdjDown;

        /* The initial and last runs are partial, because Y advances only 0.5
         * for these runs, rather than 1. Divide one full run, plus the initial
         * pixel, between the initial and last runs. */
        run_len = min_run_len / 2 + 1;
        final_run_len = run_len;

        /* If there is an odd number of pixels per run, we have one pixel that
         * can't be allocated to either the initial or last partial run, so
         * we'll add 0.5 to the error term so this pixel will be handled by the
         * normal full-run loop. */
        /* Is the run length odd? */
        if (min_run_len & 1)
        {
            /* Odd length, add YDelta to error term (add 0.5 of a pixel to the
             * error term) */
            errorAdj += YDelta;
        }
        else
        {
            /* The basic run length is even */
            /* If there's no fractional advance, we have one pixel that could
             * go to either the initial or last partial run, which we'll
             * arbitrarily allocate to the last run.*/
            if (AdjUp == 0)
            {
                --run_len;
            }
        }

        /* X-Major adjustments done now */

        /* Draw the first partial run of pixels. */
        /* XXX TODO extract out the x2 x1 choice to the outside of the loop. */
        /* Option: make an x variable we set when we choose XAdvance, and then
         * always use XAdvance as "1" when doing x-major
         * Another choice is to have a horizontal line drawing function that
         * takes Xadvance as a param, to pick to go left or right */
        if (XAdvance < 0)
        {
            /* Right to left */
            DrawHorizLine(x1-run_len+1, y1++, run_len);
            x1 -= run_len;
        }
        else
        {
            /* Left to right */
            DrawHorizLine(x1, y1++, run_len);
            x1 += run_len;
        }

        /* Draw all full runs */
        /* Are there more than 2 scans? so there are some full runs? */
        if (YDelta < 2)
        {
            /* No, no full runs */
            goto xmajor_final;
        }

        while (YDelta-- > 1) {
            run_len = min_run_len; /* Run is at least this long */
            /* Advance the error term and add an extra pixel if the error term
             * so indicates. */
            errorAdj += AdjUp;
            if (errorAdj > 0)
            {
                /* One extra pixel in run */
                ++run_len;
                /* Reset the error term */
                errorAdj -= AdjDown;
            }
            if (XAdvance < 0)
            {
                /* Right to left */
                drawHorizLine(x1-run_len+1, y1++, run_len);
                x1 -= run_len;
            }
            else
            {
                /* Left to right */
                drawHorizLine(x1, y1++, run_len);
                x1 += run_len;
            }
        }
xmajor_final:
        if (XAdvance < 0)
        {
            /* Right to left */
            drawHorizLine(x1-final_run_len+1, y1, final_run_len);
        }
        else
        {
            /* Left to right */
            drawHorizLine(x1, y1, final_run_len);
        }
        return;
    }
    else
    {
        /* Y-major */
        /* More vertical than horizontal */
        min_run_len = YDelta / XDelta;
        errorAdj = YDelta % XDelta;
        AdjUp = errorAdj + errorAdj;
        AdjDown = XDelta + XDelta;

        /* Initial error term; reflects an initial step of 0.5 along the X
         * axis */
        errorAdj -= AdjDown;

        /* The initial and last runs are partial, because X advances only 0.5
         * for these runs, rather than 1. Divide one full run, plus the initial
         * pixel, between the initial and last runs. */
        run_len = min_run_len / 2 + 1;
        final_run_len = run_len;

        /* If there is an odd number of pixels per run, we have one pixel that
         * can't be allocated to either the initial or last partial run, so
         * we'll add 0.5 to the error term so this pixel will be handled by the
         * normal full-run loop. */
        /* Is the run length odd? */
        if (min_run_len & 1)
        {
            /* Odd length, add XDelta to error term (add 0.5 of a pixel to the
             * error term) */
            errorAdj += XDelta;
        }
        else
        {
            /* The basic run length is even */
            /* If there's no fractional advance, we have one pixel that could
             * go to either the initial or last partial run, which we'll
             * arbitrarily allocate to the last run.*/
            if (AdjUp == 0)
            {
                --run_len;
            }
        }

        /* Y-Major adjustments done now */

        /* Draw the first partial run of pixels. */
        /*printf("x1,y1,len: (%d,%d,%d)\r", x1, y1, run_len);*/
        drawVertLine(x1, y1, run_len);
        y1 += run_len;
        x1 += XAdvance;

        /* Draw all full runs */
        /* Are there more than 2 scans? so there are some full runs? */
        if (XDelta < 2)
        {
            /* No, no full runs */
            goto ymajor_final;
        }

        while (XDelta-- > 1) {
            run_len = min_run_len; /* Run is at least this long */
            /* Advance the error term and add an extra pixel if the error term
             * so indicates. */
            errorAdj += AdjUp;
            if (errorAdj > 0)
            {
                /* One extra pixel in run */
                ++run_len;
                /* Reset the error term */
                errorAdj -= AdjDown;
            }
            /*printf("x1,y1,len: (%d,%d,%d)\r", x1, y1, run_len);*/
            drawVertLine(x1, y1, run_len);
            y1 += run_len;
            x1 += XAdvance;
        }
ymajor_final:
        /*printf("x1,y1,len: (%d,%d,%d)\r", x1, y1, run_len);*/
        drawVertLine(x1, y1, final_run_len);
        return;
    }
}

static void writePixel(int x, int y)
{
    unsigned long dest = VGA_VIDEO_SEGMENT;
    unsigned short col_in_byte;
    unsigned char pixel;

    /* Adjust clipping */
    if (x < clip.left || x > clip.right || y < clip.top
        || y > clip.bottom)
    {
        return;
    }

    /* Calculate start byte. */
    /* 640 lines. 80 bytes each line. */
    dest = linear_ptr(VGA_VIDEO_SEGMENT, y * 80 + x / 8);
    col_in_byte = x % 8; /* keep only the column in-byte address */

    /* Calculate mask for pixel. */
    pixel = 0x80U >> col_in_byte;

    _farpeekb(_dos_ds, dest); /* Load latches */
    _farpokeb(_dos_ds, dest, pixel);
}

static void circlePoints(int x0, int y0, int x, int y)
{
    writePixel(x0 + x, y0 + y);
    writePixel(x0 + y, y0 + x);
    writePixel(x0 + y, y0 - x);
    writePixel(x0 + x, y0 - y);
    writePixel(x0 - x, y0 - y);
    writePixel(x0 - y, y0 - x);
    writePixel(x0 - y, y0 + x);
    writePixel(x0 - x, y0 + y);
}

void DrawCircle(int color, int x0, int y0, int radius)
{
    int x;
    int y;
    int d;
    int deltaE;
    int deltaNE;

    /* Set Graphics Controller into write mode 3 */
    currentMode = 3;
    set_gc(GC_GRAPHICS_MODE, get_gc(GC_GRAPHICS_MODE) | 3);

    /* Enable all planes */
    set_seq(SEQ_MAP_MASK, 0xF);

    /* Set color in set/reset reg */
    set_gc(GC_SET_RESET, color);

    set_gc(GC_BIT_MASK, 0xFF);

    /* Speed this up by accumulating horizontal, diagonal, and vertical line
     * segments. Draw each line segment reflected around in each octant. */

    x = 0;
    y = radius;
    d = 1 - radius;
    deltaE = 3;
    deltaNE = -2 * radius + 5;
    circlePoints(x0, y0, x, y);

    while (y > x)
    {
        if (d < 0)
        {
            /* East */
            d += deltaE;
            deltaE += 2;
            deltaNE += 2;
        }
        else
        {
            /* Northeast */
            d += deltaNE;
            deltaE += 2;
            deltaNE += 4;
            --y;
        }
        ++x;
        circlePoints(x0, y0, x, y);
    }
}

static void fillCirclePoints(int x0, int y0, int x, int y)
{
    /* NW to NE */
    drawHorizLine(x0 - x, y0 - y, x + x); /* Top */
    drawHorizLine(x0 - y, y0 - x, y + y); /* Upper middle */

    /* SW to SE */
    drawHorizLine(x0 - y, y0 + x, y + y); /* Lower middle */
    drawHorizLine(x0 - x, y0 + y, x + x); /* Bottom */
}

void FillCircle(int color, int x0, int y0, int radius)
{
    int x;
    int y;
    int d;
    int deltaE;
    int deltaNE;

    /* Set Graphics Controller into write mode 3 */
    currentMode = 3;
    set_gc(GC_GRAPHICS_MODE, get_gc(GC_GRAPHICS_MODE) | 3);

    /* Enable all planes */
    set_seq(SEQ_MAP_MASK, 0xF);

    /* Set color in set/reset reg */
    set_gc(GC_SET_RESET, color);

    set_gc(GC_BIT_MASK, 0xFF);

    /* Speed this up by accumulating horizontal, diagonal, and vertical line
     * segments. Draw each line segment reflected around in each octant. */
    /* Speed this up by not drawing the same pixels multiple times. (Currently,
     * any horizontal lines at the top of the circle are drawn over many times,
     * as a specific example.) */

    x = 0;
    y = radius;
    d = 1 - radius;
    deltaE = 3;
    deltaNE = -2 * radius + 5;
    fillCirclePoints(x0, y0, x, y);

    while (y > x)
    {
        if (d < 0)
        {
            /* East */
            d += deltaE;
            deltaE += 2;
            deltaNE += 2;
        }
        else
        {
            /* Northeast */
            d += deltaNE;
            deltaE += 2;
            deltaNE += 4;
            --y;
        }
        ++x;
        fillCirclePoints(x0, y0, x, y);
    }
}

static void roundPoints(int x0, int y0, int radius, int x, int y, int width, int height)
{
    /* SE */
    writePixel(x0 + x + width - radius - 1, y0 + y + height - radius - 1);
    writePixel(x0 + y + width - radius - 1, y0 + x + height - radius - 1);

    /* NE */
    writePixel(x0 + y + width - radius - 1, y0 - x + radius);
    writePixel(x0 + x + width - radius - 1, y0 - y + radius);

    /* NW */
    writePixel(x0 - x + radius, y0 - y + radius);
    writePixel(x0 - y + radius, y0 - x + radius);

    /* SW */
    writePixel(x0 - y + radius, y0 + x + height - radius - 1);
    writePixel(x0 - x + radius, y0 + y + height - radius - 1);
}

void DrawRoundRect(int color, int x0, int y0, int radius, int width, int height)
{
    int x;
    int y;
    int d;
    int deltaE;
    int deltaNE;

    /* TODO Fix crash when radius is more curvy than a circle is allowed to be
     * */

    /* TODO Maybe come up with the concept of a "pen" that has properties
     * set on it. Make it so that when pen properties are set, the VGA reg are
     * loaded (only works when there is one pen, not multiple, without further
     * VGA reg tracking for differences). */
    setMode3Color(color);

    /* Draw rectangle portion, without corners */
    drawVertLine(x0, y0 + radius + 1, height - 2 - 2 * radius); /* Left */
    drawHorizLine(x0 + 1 + radius, y0, width - 2 - 2 * radius); /* Top */
    drawVertLine(x0 + width - 1, y0 + radius + 1,
                 height - 2 - 2 * radius); /* Right */
    drawHorizLine(x0 + 1 + radius, y0 + height - 1,
                  width - 2 - 2 * radius); /* Bottom */

    /* Same as DrawCircle(), but with roundPoints() */
    x = 0;
    y = radius;
    d = 1 - radius;
    deltaE = 3;
    deltaNE = -2 * radius + 5;
    roundPoints(x0, y0, radius, x, y, width, height);

    while (y > x)
    {
        if (d < 0)
        {
            /* East */
            d += deltaE;
            deltaE += 2;
            deltaNE += 2;
        }
        else
        {
            /* Northeast */
            d += deltaNE;
            deltaE += 2;
            deltaNE += 4;
            --y;
        }
        ++x;
        roundPoints(x0, y0, radius, x, y, width, height);
    }
}

static void fillRoundPoints(int x0, int y0, int radius, int x, int y, int width, int height)
{
    /* NW to NE */
    drawHorizLine(x0 - x + radius, y0 - y + radius,
                  (x0 + x + width - radius) - (x0 - x + radius)); /* Top */
    drawHorizLine(x0 - y + radius, y0 - x + radius,
                  (x0 + y + width - radius) -
                      (x0 - y + radius)); /* Upper middle */

    /* SW to SE */
    drawHorizLine(x0 - y + radius, y0 + x + height - radius - 1,
                  (x0 + y + width - radius) -
                      (x0 - y + radius)); /* Lower middle */
    drawHorizLine(x0 - x + radius, y0 + y + height - radius - 1,
                  (x0 + x + width - radius) - (x0 - x + radius)); /* Bottom */
}

void FillRoundRect(int color, int x0, int y0, int radius, int width,
                   int height)
{
    int x;
    int y;
    int d;
    int deltaE;
    int deltaNE;
    struct Rect rect;

    /* Use FillRect() to set mode for us, and avoid extra register setting */
    rect.left = x0;
    rect.top = y0 + radius + 1;
    rect.right = rect.left + width - 1;
    rect.bottom = rect.top + height - radius - radius - 2 - 1;
    FillRect(color, &rect); /* Middle */

    x = 0;
    y = radius;
    d = 1 - radius;
    deltaE = 3;
    deltaNE = -2 * radius + 5;
    fillRoundPoints(x0, y0, radius, x, y, width, height);

    while (y > x)
    {
        if (d < 0)
        {
            /* East */
            d += deltaE;
            deltaE += 2;
            deltaNE += 2;
        }
        else
        {
            /* Northeast */
            d += deltaNE;
            deltaE += 2;
            deltaNE += 4;
            --y;
        }
        ++x;
        fillRoundPoints(x0, y0, radius, x, y, width, height);
    }
}

#if 0
static unsigned char apply_op(unsigned char byte, int op,
                              const unsigned char *pattern, size_t row)
{
    unsigned char p = pattern[row % 8];
    switch (op)
    {
    case OP_SRC_INV:
        byte = ~byte;
        break;
    case OP_SRC_INV_PAT_XOR:
        byte = ~byte ^ p;
        break;
    case OP_PAT_AND:
        byte &= p;
        break;
    case OP_SRC_INV_PAT_AND:
        byte = ~byte & p;
        break;
    case OP_PAT_NOT:
        byte &= ~p;
        break;
    case OP_PAT_OR:
        byte |= p;
        break;
    case OP_SRC_INV_PAT_OR:
        byte = ~byte | p;
        break;
    case OP_PAT_XOR:
        byte ^= p;
        break;
    case OP_NONE:
    default:
        break;
    }

    return byte;
}
#endif

static unsigned char get_op(int op, const unsigned char *src, size_t i,
                            const unsigned char *pat, size_t row)
{
    unsigned char p = pat[row % 8];
#if 0
    unsigned char byte = ~src[i] & 0xFF; /* ?!?! */
#endif
    unsigned char byte = src[i] & 0xFF; /* ?!?! */
    switch (op)
    {
    case OP_SRC_INV:
        byte = ~byte;
        break;
    case OP_SRC_INV_PAT_XOR:
        byte = ~byte ^ p;
        break;
    case OP_PAT_AND:
        byte &= p;
        break;
    case OP_SRC_INV_PAT_AND:
        byte = ~byte & p;
        break;
    case OP_PAT_NOT:
        byte &= ~p;
        break;
    case OP_PAT_OR:
        byte |= p;
        break;
    case OP_SRC_INV_PAT_OR:
        byte = ~byte | p;
        break;
    case OP_PAT_XOR:
        byte ^= p;
        break;
    case OP_NONE:
    default:
        break;
    }

    return byte;
}

int Blit(const unsigned char *img, const struct Rect *dst0,
         const struct Rect *src0, int span)
{
    return BlitOp(img, OP_NONE, dst0, src0, span);
}

int BlitOp(const unsigned char *img, int op, const struct Rect *dst0,
           const struct Rect *src0, int span)
{
    unsigned long dest;
    unsigned short odcol_in_byte;
    unsigned short oscol_in_byte;
    unsigned long bits;
    unsigned long mask_bits;
    int line;
    size_t row;
    size_t i;
    int ret;
    struct Rect dst;
    struct Rect src;

    /* Adjust clipping */
    dst = *dst0;
    src = *src0;
    ret = ClipRectAdjust(&dst, &src, &clip);
    if (ret == CLIP_REJECTED)
    {
        return 0;
    }

    /* Calculate start byte. */
    /* 640 lines. 80 bytes each line. */
    dest = linear_ptr(VGA_VIDEO_SEGMENT, dst.top * 80 + dst.left / 8);
    odcol_in_byte = (dst.left % 8); /* keep only the column in-byte address */
    oscol_in_byte = (src.left % 8); /* keep only the column in-byte address */

    span = (span + 7) / 8; /* Convert to bytes */

    row = src.top;
    for (line = src.top * span + src.left / 8;
         line <= src.bottom * span + src.left / 8; line += span)
    {
        unsigned char srcbits;
        unsigned char srcbit;
        unsigned char mbit;
        int pixels;
        unsigned short dcol_in_byte;
        unsigned short scol_in_byte;
        size_t written;

        dcol_in_byte = odcol_in_byte;
        scol_in_byte = oscol_in_byte;
        pixels = src.right - src.left + 1;
        bits = 0; /* Collects bits from img */
        mask_bits = 0; /* Collects bits from mask */
        i = 0;
        srcbits = get_op(op, img, line + i, currentPattern, row);
        written = 0;
        while (pixels--)
        {
            if (scol_in_byte == 8)
            {
                /* We have exhausted the img byte. Read another. */
                ++i;
                srcbits = get_op(op, img, line + i, currentPattern, row);
                scol_in_byte = 0;
            }

            srcbit = srcbits & (0x80U >> scol_in_byte);
            srcbit <<= scol_in_byte; /* Reset bit back to MSB. */
            srcbit >>= dcol_in_byte; /* Shift into destination bit location. */
            bits |= srcbit;
            mbit = 0xFF & (0x80U >> scol_in_byte);
            mbit <<= scol_in_byte; /* Reset bit back to MSB. */
            mbit >>= dcol_in_byte; /* Shift into destination bit location. */
            mask_bits |= mbit;

            if ((dcol_in_byte == 7) || (pixels == 0))
            {
                /* We have collected a full byte. Write it. */
                /* Optimization idea: Might be faster to sort into
                same masks, then write runs with common masks, rather than
                write the slow VGA register every byte */
                /* XXX Do left side, middle, and right in 3 loops */
                /*set_gc(GC_BIT_MASK, mask_bits);*/

                /*bits = apply_op(bits, op, currentPattern, row);*/

                _farpeekb(_dos_ds, dest); /* Load latches */
                _farpokeb(_dos_ds, dest++, bits);
                ++written;
                bits = 0;
                mask_bits = 0;
                dcol_in_byte = 0;
                ++scol_in_byte;
                continue;
            }

            ++scol_in_byte;
            ++dcol_in_byte;
        }

        dest += 80 - written;
        ++row;
    }

    return 0;
}

int BlitWithMask(const unsigned char *img, const unsigned char *mask,
                 const struct Rect *dst0, const struct Rect *src0,
                 int span)
{
    unsigned long dest;
    unsigned short odcol_in_byte;
    unsigned short oscol_in_byte;
    unsigned long bits;
    unsigned long mask_bits;
    int line;
    size_t i;
    int ret;
    struct Rect dst;
    struct Rect src;

    /* Adjust clipping */
    dst = *dst0;
    src = *src0;
    ret = ClipRectAdjust(&dst, &src, &clip);
    if (ret == CLIP_REJECTED)
    {
        return 0;
    }

    /* Calculate start byte. */
    /* 640 lines. 80 bytes each line. */
    dest = linear_ptr(VGA_VIDEO_SEGMENT, dst.top * 80 + dst.left / 8);
    odcol_in_byte = (dst.left % 8); /* keep only the column in-byte address */
    oscol_in_byte = (src.left % 8); /* keep only the column in-byte address */

    span = (span + 7) / 8; /* Convert to bytes */

    for (line = src.top * span + src.left / 8;
         line <= src.bottom * span + src.left / 8; line += span)
    {
        unsigned char srcbits;
        unsigned char srcbit;
        unsigned char mbits;
        unsigned char mbit;
        int pixels;
        unsigned short dcol_in_byte;
        unsigned short scol_in_byte;
        size_t written;

        dcol_in_byte = odcol_in_byte;
        scol_in_byte = oscol_in_byte;
        pixels = src.right - src.left + 1;
        bits = 0; /* Collects bits from img */
        mask_bits = 0; /* Collects bits from mask */
        i = 0;
        srcbits = img[line + i];
        mbits = mask[line + i];
        written = 0;
        while (pixels--)
        {
            if (scol_in_byte == 8)
            {
                /* We have exhausted the img byte. Read another. */
                ++i;
                srcbits = img[line + i];
                mbits = mask[line + i];
                scol_in_byte = 0;
            }

            srcbit = srcbits & (0x80U >> scol_in_byte);
            srcbit <<= scol_in_byte; /* Reset bit back to MSB. */
            srcbit >>= dcol_in_byte; /* Shift into destination bit location. */
            bits |= srcbit;
            mbit = mbits & (0x80U >> scol_in_byte);
            mbit <<= scol_in_byte; /* Reset bit back to MSB. */
            mbit >>= dcol_in_byte; /* Shift into destination bit location. */
            mask_bits |= mbit;

            if ((dcol_in_byte == 7) || (pixels == 0))
            {
                /* We have collected a full byte. Write it. */

                /* Optimization idea: Might be faster to sort into
                same masks, then write runs with common masks, rather than
                write the slow VGA register every byte */
                set_gc(GC_BIT_MASK, mask_bits);

                _farpeekb(_dos_ds, dest); /* Load latches */
                _farpokeb(_dos_ds, dest++, bits);
                ++written;
                bits = 0;
                mask_bits = 0;
                dcol_in_byte = 0;
                ++scol_in_byte;
                continue;
            }

            ++scol_in_byte;
            ++dcol_in_byte;
        }

        dest += 80 - written;
    }

    return 0;
}

int DrawBitmap(const struct Rect *dst, int span, const unsigned char *img,
               const unsigned char *mask, int color)
{
    struct Rect src;

    src.left = 0;
    src.top = 0;
    src.right = dst->right - dst->left;
    src.bottom = dst->bottom - dst->top;

#if DEBUG
    SerialPrintf("dst: (%d,%d)(%d,%d)\n", dst->left, dst->top, dst->right,
                 dst->bottom);
    SerialPrintf("src: (%d,%d)(%d,%d)\n", src.left, src.top, src.right,
                 src.bottom);
#endif

    /* Set Graphics Controller into write mode 0 */
    currentMode = 0;
    set_gc(GC_GRAPHICS_MODE, get_gc(GC_GRAPHICS_MODE) & ~0x03);

    /* Enable all planes */
    set_seq(SEQ_MAP_MASK, 0xF);

    /* Turn black into color, everything else white */
    /* For each bit that is 0, CPU byte goes through OK in that plane. */
    set_gc(GC_ENABLE_SET_RESET, color);
    /* For other bits, we use set/reset to force them to 1. */
    set_gc(GC_SET_RESET, 0xFU);

    /* We don't need to set GC_BIT_MASK to anything, as the *Mask() function
     * will do that as needed. */
    return BlitWithMask(img, mask, dst, &src, span);
}

/* Input pixel format is planar 4bpp */
int DrawColorBitmap(const struct Rect *dst, int span, const unsigned char *img,
                    const unsigned char *mask)
{
    enum { PLANES = 4 };
    int sb = (span + 7) / 8; /* Convert to bytes */
    int h = dst->bottom - dst->top + 1;
    int planeSize = sb * h;
    int i;
    struct Rect src;

    src.left = 0;
    src.top = 0;
    src.right = dst->right - dst->left;
    src.bottom = h - 1;

    /* Set Graphics Controller into write mode 0 */
    currentMode = 0;
    set_gc(GC_GRAPHICS_MODE, get_gc(GC_GRAPHICS_MODE) & ~0x3U);

    /* Disable set/reset */
    set_gc(GC_ENABLE_SET_RESET, 0);

    /* Blit each plane separately, using the SEQ_MAP_MASK to control which
     * plane/bank of VRAM is written to. */
    for (i = 0; i < PLANES; ++i)
    {
        set_seq(SEQ_MAP_MASK, 1 << i);
        BlitWithMask(&img[i * planeSize], mask, dst, &src, span);
    }

    return 0;
}

int SaveScreenShot(const char *path)
{
    (void)path;
    return 0;
}
