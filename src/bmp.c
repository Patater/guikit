/*
 *  bmp.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2020-12-24.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/bmp.h"
#include "guikit/primrect.h"
#include "guikit/ptypes.h"
#include "guikit/debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct bmp
{
    u8 type[2];
    u32 file_size;
    u32 reserved;
    u32 pixels_offset;
} __attribute__((packed));

struct dib
{
    u32 size;
    i32 width;
    i32 height;
    i16 planes;
    i16 bpp; /* Bits per pixel */

    /* BMP Version 3 */
    u32 compression;
    u32 image_size;
    i32 hres;
    i32 vres;
    u32 num_colors; /* Number of colors in palette */
    u32 num_important;
} __attribute__((packed));

enum {
    DIB_COMPRESSION_NONE = 0,
    DIB_COMPRESSION_RLE8 = 1,
    DIB_COMPRESSION_RLE4 = 2
};

static int load_bmp(struct bmp *bmp, FILE *f)
{
    size_t num_read;

    /* Load the bitmap header */
    num_read = fread(bmp, 1, sizeof(*bmp), f);
    if (num_read != sizeof(*bmp))
    {
        DebugPrintf("Header size mismatch\n");
        return -1;
    }

    /* Check BMP type */
    if (bmp->type[0] != 'B' && bmp->type[1] != 'M')
    {
        /* Unknown type */
        DebugPrintf("Unknown type\n");
        return -1;
    }

    return 0;
}

static int load_dib(struct dib *dib, FILE *f)
{
    size_t num_read;

    /* Load the DIB */
    /* Note: this might read more than the actual DIB size. We'll just attempt
     * to read what we expect and if the size ends up being bad, we bail. This
     * allows us to get away with one fread() call, instead of 2, to load a
     * DIB. */
    num_read = fread(dib, 1, sizeof(*dib), f);
    if (num_read != sizeof(*dib))
    {
        return -1;
    }

    /* Check DIB header size */
    if (dib->size < sizeof(*dib))
    {
        /* Unsupported DIB size */
        return -1;
    }

    return 0;
}

void dump_bmp(struct bmp *bmp)
{
    DebugPrintf("type: %c%c\n", bmp->type[0], bmp->type[1]);
    DebugPrintf("file size: %lu\n", bmp->file_size);
    DebugPrintf("pixels offset: 0x%04lX\n", bmp->pixels_offset);
}

void dump_dib(struct dib *dib)
{
    unsigned long num_entries;

    DebugPrintf("size: %lu\n", dib->size);
    DebugPrintf("width: %ld\n", dib->width);
    DebugPrintf("height: %ld\n", dib->height);
    DebugPrintf("planes: %d\n", dib->planes);
    DebugPrintf("bpp: %d\n", dib->bpp);

    if (dib->size < 40)
    {
        DebugPrintf("Old-style bitmap\n");
        return;
    }

    DebugPrintf("compression: %lu\n", dib->compression);
    DebugPrintf("image_size: %lu\n", dib->image_size);
    DebugPrintf("hres: %lu\n", dib->hres);
    DebugPrintf("vres: %lu\n", dib->vres);
    DebugPrintf("num colors: %lu\n", dib->num_colors);
    num_entries = 1 << dib->bpp;
    DebugPrintf("num entries: %lu\n", num_entries);
    DebugPrintf("palette:\n");
    if (dib->bpp >= 16)
    {
        DebugPrintf("\tNone\n");
    }
}

void dump_palette(unsigned long num_entries, const unsigned char *palette)
{
    enum { ENTRY_SIZE = 4};
    unsigned int i;

    for (i = 0; i < num_entries * ENTRY_SIZE; i += ENTRY_SIZE)
    {
        unsigned long color =
            palette[i] << 16 |
            palette[i + 1] << 8 |
            palette[i + 2];
        DebugPrintf("\t%d - #%06lX\n", i / ENTRY_SIZE, color);
    }
}

static int dib_image_size(const struct dib *dib)
{
    size_t image_size;
    if (dib->image_size == 0)
    {
        /* If image_size is 0, we have to compute this ourselves. */
        /* Sometimes height can be negative. This means that the bitmap isn't
         * upside down. */
        image_size = dib->width * labs(dib->height) * dib->bpp / 8;
    }
    else
    {
        image_size = dib->image_size;
    }

    return image_size;
}

static unsigned char *convert_bmp_bw(const struct bmp *bmp,
                                     const struct dib *dib, FILE *f)
{
    size_t image_size;
    long img_width;
    long num_lines;
    unsigned long line_len;
    unsigned char *img = NULL;
    unsigned char *dst = NULL;
    unsigned char *line = NULL;
    int status;

    /* Allocate enough space for one line of the pixels to use as a buffer.
     * First, determine how many bytes are needed for all pixels in a line.
     * Then, round up that many bytes to the nearest multiple of 4 (to ensure
     * the line length is a multiple 32 bits) */
    line_len = ((((dib->width * dib->bpp + 7) / 8) + 3) / 4) * 4;
    line = malloc(line_len);
    if (!line)
    {
        DebugPrintf("Out of mem\n");
        goto error;
    }

    img_width = (dib->width * dib->bpp + 7) / 8;
    num_lines = dib->height;
    image_size = img_width * num_lines;

    /* Enforce a cap on image size */
    if (image_size >= 4096)
    {
        /* Too big of image */
        DebugPrintf("Too big of an image\n");
        goto error;
    }

    /* Allocate space for the image we'll keep around for a while. */
    img = malloc(image_size);
    if (!img)
    {
        DebugPrintf("Out of mem\n");
        goto error;
    }


    status = fseek(f, bmp->pixels_offset, SEEK_SET);
    if (status != 0)
    {
        /* Couldn't go to pixels */
        DebugPrintf("Couldn't go to pixels\n");
        goto error;
    }

    dst = img + image_size - img_width;
    while (num_lines--)
    {
        size_t num_read;

        num_read = fread(line, 1, line_len, f);
        if (num_read != line_len)
        {
            DebugPrintf("Read error\n");
            goto error;
        }

        /* Copy only the meaningful bits of the line (no padding). */
        /* Maybe we should copy the padding? */
        /* We currently have padding of up to 1 byte only, rather than always 4
         * bytes */
        memcpy(dst, line, img_width);
        dst -= img_width;
    }

    free(line);
    return img;

error:
    free(line);
    free(img);
    return NULL;
}

static void write_planar(unsigned char *dst, unsigned char *src,
                         unsigned long width, size_t plane_size)
{
    /* Width in bytes 8*/

    /* Given a chunky bitmap, 2 values per byte, write to dst in planar form, 8
     * values per byte. */
    /* Assume we start (left edge) aligned within a byte */
    unsigned char plane_0;
    unsigned char plane_1;
    unsigned char plane_2;
    unsigned char plane_3;
    unsigned long i;

    for (i = 0; i < width; ++i)
    {
        plane_0 = 0;
        plane_1 = 0;
        plane_2 = 0;
        plane_3 = 0;

        /* 0 */
        plane_3 |= (src[0] & 0x80U) << 0;
        plane_2 |= (src[0] & 0x40U) << 1;
        plane_1 |= (src[0] & 0x20U) << 2;
        plane_0 |= (src[0] & 0x10U) << 3;

        /* 1 */
        plane_3 |= (src[0] & 0x08U) << 3;
        plane_2 |= (src[0] & 0x04U) << 4;
        plane_1 |= (src[0] & 0x02U) << 5;
        plane_0 |= (src[0] & 0x01U) << 6;

        /* 2 */
        plane_3 |= (src[1] & 0x80U) >> 2;
        plane_2 |= (src[1] & 0x40U) >> 1;
        plane_1 |= (src[1] & 0x20U) >> 0;
        plane_0 |= (src[1] & 0x10U) << 1;

        /* 3 */
        plane_3 |= (src[1] & 0x08U) << 1;
        plane_2 |= (src[1] & 0x04U) << 2;
        plane_1 |= (src[1] & 0x02U) << 3;
        plane_0 |= (src[1] & 0x01U) << 4;

        /* 4 */
        plane_3 |= (src[2] & 0x80U) >> 4;
        plane_2 |= (src[2] & 0x40U) >> 3;
        plane_1 |= (src[2] & 0x20U) >> 2;
        plane_0 |= (src[2] & 0x10U) >> 1;

        /* 5 */
        plane_3 |= (src[2] & 0x08U) >> 1;
        plane_2 |= (src[2] & 0x04U) >> 0;
        plane_1 |= (src[2] & 0x02U) << 1;
        plane_0 |= (src[2] & 0x01U) << 2;

        /* 6 */
        plane_3 |= (src[3] & 0x80U) >> 6;
        plane_2 |= (src[3] & 0x40U) >> 5;
        plane_1 |= (src[3] & 0x20U) >> 4;
        plane_0 |= (src[3] & 0x10U) >> 3;

        /* 7 */
        plane_3 |= (src[3] & 0x08U) >> 3;
        plane_2 |= (src[3] & 0x04U) >> 2;
        plane_1 |= (src[3] & 0x02U) >> 1;
        plane_0 |= (src[3] & 0x01U) >> 0;

        /* TODO Could be faster if we take advantage of bmp data being padded
         * for 32-bit. We could then always do a 32-bit write, instead of byte
         * at a time. We could also swizzle more bits at once, too. This would
         * be at the expense of using more RAM for stored bitmaps, though. */
        dst[plane_size * 0] = plane_0;
        dst[plane_size * 1] = plane_1;
        dst[plane_size * 2] = plane_2;
        dst[plane_size * 3] = plane_3;

        src += 4;
        ++dst;
    }
}

static unsigned char *convert_bmp_color(const struct bmp *bmp,
                                        const struct dib *dib,
                                        FILE *f)
{
    size_t image_size;
    size_t plane_size;
    long img_width;
    long num_lines;
    unsigned long line_len;
    unsigned char *img = NULL;
    unsigned char *dst = NULL;
    unsigned char *line = NULL;
    int status;

    image_size = dib_image_size(dib);

    /* Enforce a cap on image size */
    if (image_size >= 4096)
    {
        /* Too big of image */
        goto error;
    }

    /* Allocate enough space for one line of the pixels to use as a buffer.
     * First, determine how many bytes are needed for all pixels in a line.
     * Then, round up that many bytes to the nearest multiple of 4 (to ensure
     * the line length is a multiple 32 bits) */
    line_len = ((((dib->width * dib->bpp + 7) / 8) + 3) / 4) * 4;
    line = malloc(line_len);
    if (!line)
    {
        goto error;
    }

    /* Allocate space for the image we'll keep around for a while. */
    img = malloc(image_size);
    if (!img)
    {
        goto error;
    }


    status = fseek(f, bmp->pixels_offset, SEEK_SET);
    if (status != 0)
    {
        /* Couldn't go to pixels */
        goto error;
    }

    num_lines = dib->height;
    img_width = (dib->width + 7) / 8;
    plane_size = img_width * dib->height;
    dst = img + plane_size - img_width;
    while (num_lines--)
    {
        size_t num_read;

        num_read = fread(line, 1, line_len, f);
        if (num_read != line_len)
        {
            goto error;
        }

        /* Copy only the meaningful bits of the line (no padding). */
        write_planar(dst, line, img_width, plane_size);
        dst -= img_width;
    }

    free(line);
    return img;

error:
    free(line);
    free(img);
    return NULL;
}

unsigned char *bmp_load(const char *path, struct Rect *rect, short *planes)
{
    FILE *f;
    unsigned char *img = NULL;
    unsigned char palette[4*16];
    int status;
    size_t num_read;
    struct bmp bmp;
    struct dib dib;

    f = fopen(path, "rb");
    if (!f) {
        DebugPrintf("No such file %s\n", path);
        return NULL;
    }

    status = load_bmp(&bmp, f);
    if (status)
    {
        goto error;
    }
    /*dump_bmp(&bmp);*/

    status = load_dib(&dib, f);
    if (status)
    {
        goto error;
    }
    /*dump_dib(&dib);*/

    /* Dump palette */
    if (dib.bpp < 16)
    {
        enum { ENTRY_SIZE = 4};
        unsigned long num_entries;

        num_entries = 1 << dib.bpp;

        status = fseek(f, sizeof(bmp) + dib.size, SEEK_SET);
        if (status != 0)
        {
            /* Couldn't go to palette */
            goto error;
        }

        num_read = fread(palette, 1, num_entries * ENTRY_SIZE, f);
        if (num_read != num_entries * ENTRY_SIZE)
        {
            goto error;
        }

        /*dump_palette(num_entries, palette);*/
    }

    /* TODO RLE decoding */
    if (dib.compression != DIB_COMPRESSION_NONE)
    {
        /* Unsupported compression */
        goto error;
    }

    /*
     * Convert BMP raw pixels to image in the format we want.
     */
    switch(dib.num_colors)
    {
    case 2:
        img = convert_bmp_bw(&bmp, &dib, f);
        break;
    case 16:
        img = convert_bmp_color(&bmp, &dib, f);
        break;
    default:
        goto error;
    }

    if (!img)
    {
        goto error;
    }

    rect->left = 0;
    rect->top = 0;
    rect->right = dib.width - 1;
    rect->bottom = labs(dib.height) - 1;
    *planes = dib.num_colors == 16 ? 4 : 1;

    fclose(f);
    return img;
error:
    fclose(f);
    free(img);
    return NULL;
}

void bmp_free(unsigned char *img)
{
    free(img);
}
