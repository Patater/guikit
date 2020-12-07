/*
 *  graphics.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2020-12-12.
 *  Copyright 2020. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/graphics.h"
#include "guikit/primrect.h"

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

static void setMode3Color(int color)
{
    /* Set Graphics Controller into write mode 3 */
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

    /* Reset the data rotate and bit mask registers. */
    /* XXX Maybe not needed after mode set. */
    set_gc(GC_DATA_ROTATE, 0);
    set_gc(GC_BIT_MASK, 0xFF);

    FillScreen(COLOR_BLACK);

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

void DrawRect(int color, int x1, int y1, int width, int height)
{
    setMode3Color(color);

    drawVertLine(x1, y1, height); /* Left */
    drawHorizLine(x1+1, y1, width-1); /* Top */
    drawVertLine(x1+width-1, y1+1, height-1); /* Right */
    drawHorizLine(x1+1, y1+height-1, width - 2); /* Bottom */
}

void FillRect(int color, int x1, int y1, int width, int height)
{
    int y;

    setMode3Color(color);

    /* Fill in everywhere */
    /* TODO do this quicker by doing this inline, having one loop instead of
     * two. Handle the rectangle left and right edge masking here, too. */
    for (y = 0; y < height; ++y)
    {
        drawHorizLine(x1, y1+y, width);
    }
}

void FillRectOp(int bg_color, int fg_color, const unsigned char *pattern,
                int op, int x1, int y1, int width, int height)
{
    /* TODO */
    (void) bg_color;
    (void) pattern;
    (void) op;
    FillRect(fg_color, x1, y1, width, height);
}
