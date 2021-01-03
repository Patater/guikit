/*
 *  font.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2020-12-29.
 *  Copyright 2020. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/font.h"

#include "guikit/bmp.h"
#include "guikit/debug.h"
#include "guikit/graphics.h"
#include "guikit/hashmap.h"
#include "guikit/intern.h"
#include "guikit/panic.h"
#include "guikit/pmemory.h"
#include "guikit/primrect.h"
#include "guikit/snprintf.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct hashmap *map;
static int c; /* Current character from stream */

struct Glyph {
    unsigned char offset; /* How far to move right before drawing glyph */
    unsigned char width; /* Fully rendered size, including right space */
    unsigned short location; /* Start of glyph in bitmap */
};

struct Font
{
    int height;
    int width;
    short planes;
    unsigned char *img;
    int maxHeight;
    int leading;
    int startGlyph;
    int endGlyph;
    int numGlyphs;
    const char *name;
    size_t nameLen;
    struct Glyph *g;
};

static void consume(FILE *f)
{
    int ret;

    c = fgetc(f);
    ret = ferror(f);
    if (ret != 0)
    {
        /* File I/O error */
        /* LCOV_EXCL_START */
        panic("%s: File I/O error", "font:consume");
        /* LCOV_EXCL_STOP */
    }
}

static void ignoreWhitespace(FILE *f)
{
    /* Ignore whitespace */
    while (isspace(c))
    {
        consume(f);
    }
}

static void ignoreComments(FILE *f)
{
    while (c == ';')
    {
        /* Consume all characters until the next newline or EOF, whichever
         * comes first. */
        while (c != '\n')
        {
            consume(f);

            if (c == EOF)
            {
                return;
            }
        }
        consume(f); /* Eat the newline */
    }
}

static int isalphanum(int c)
{
    return isalpha(c) || (c >= '0' && c <= '9');
}

static char *getSection(FILE *f)
{
    enum { LEN = 33 };
    char *name = NULL;
    int i;

    /*DebugPrintf("getSection\n");*/
    ignoreWhitespace(f);
    ignoreComments(f);

    if (c != '[')
    {
        /* No opening '[' */
        goto error;
    }
    /*DebugPrintf("[");*/

    consume(f);
    name = pmalloc(LEN);
    i = 0;
    do
    {
        name[i++] = c;
        if (i >= LEN - 1)
        {
            /* We've run out of space in the buffer */
            /*DebugPrintf("\nNo space in buffer!\n");*/
            goto error;
        }
        /*DebugPrintf("%c", c);*/
        consume(f);
    } while (isalphanum(c) || c == ':');

    if (c != ']')
    {
        /* No terminating ']' */
        /*DebugPrintf("Expected ']'. Got '%c'.", c);*/
        goto error;
    }
    /*DebugPrintf("]\n");*/
    consume(f); /* Eat the ']' */

    name[i] = '\0';

    return name;

error:
    pfree(name);
    return NULL;
}

static char *getKey(FILE *f)
{
    enum { LEN = 33 };
    char *name = NULL;
    int i;

    ignoreWhitespace(f);
    ignoreComments(f);

    /*DebugPrintf("name: ");*/
    name = pmalloc(LEN);
    i = 0;
    do{
        name[i++] = c;
        if (i >= LEN - 1)
        {
            /* We've run out of space in the buffer */
            goto error;
        }
    /*    DebugPrintf("%c", c);*/
        consume(f);
    } while (isalphanum(c));

    if (c != '=')
    {
        /* No equals sign */
        goto error;
    }
    consume(f); /* Eat the '=' */
    /*DebugPrintf("\n");*/

    name[i] = '\0';

    return name;

error:
    pfree(name);
    return NULL;
}

static char *getValue(FILE *f)
{
    enum { LEN = 33 };
    char *value = NULL;
    int i;

    ignoreWhitespace(f);
    /* We are intentionally not ignoring comments here. Any ';' will be part of
     * the value. */

    /*DebugPrintf("str value: ");*/
    value = pmalloc(LEN);
    i = 0;
    do
    {
        value[i++] = c;
        if (i >= LEN - 1)
        {
            /* We've run out of space in the buffer */
            goto error;
        }
        /*    DebugPrintf("%c", c);*/
        consume(f);
    } while (!isspace(c));
    /*DebugPrintf("\n");*/

    value[i] = '\0';

    return value;

error:
    pfree(value);
    return NULL;
}

static int readIntValue(int *dst, FILE *f)
{
    char *value;
    value = getValue(f);
    if (!value)
    {
        return -1;
    }

    sscanf(value, "%6d", dst);
    pfree(value);

    return 0;
}

int readKeyPair(struct Font *font, FILE *f)
{
    const char *key;
    int ret = -1;

    key = getKey(f);
    if (!key)
    {
        return -1;
    }

    key = intern(key);

    if (key == intern("MaxHeight"))
    {
        ret = readIntValue(&font->maxHeight, f);
    }
    else if (key == intern("Leading"))
    {
        ret = readIntValue(&font->leading, f);
    }
    else if (key == intern("StartGlyph"))
    {
        ret = readIntValue(&font->startGlyph, f);
    }
    else if (key == intern("EndGlyph"))
    {
        ret = readIntValue(&font->endGlyph, f);
    }
    else if (key == intern("NumGlyphs"))
    {
        ret = readIntValue(&font->numGlyphs, f);
    }

    return ret;
}

static int readBitmapSection(struct Glyph *g, int numGlyphs, FILE *f)
{
    int i;
    const char *key;
    char *end;
    int index;
    const char *value;
    int location = 0;

    for (i = 0; i < numGlyphs; ++i)
    {
        int offset = 255;
        int width = 255;

        /* Read in each glyph triplet. */
        key = getKey(f);
        /* DebugPrintf("key: '%s'\n", key);*/
        if (key)
        {
            index = strtol(key, &end, 10);
            /* DebugPrintf("index: %d\n", index);*/
            if (*end != '\0')
            {
                /* Failed conversion */
                goto error;
            }

            if (index < i)
            {
                /* Glyph info out of order. */
                goto error;
            }

            /* Fill in the key's values */
            value = getValue(f);
            if (!value)
            {
                goto error;
            }
            offset = strtol(value, &end, 10);
            if (*end != ',')
            {
                goto error;
            }

            value = end+1;
            width = strtol(value, &end, 10);
            if (*end != ',')
            {
                goto error;
            }

            value = end+1;
            location = strtol(value, &end, 10);
            if (*end != '\0')
            {
                goto error;
            }

        }
        else
        {
            /* Ran out of indexes. Fill in the rest with default */
            index = numGlyphs - 1;
        }

        /* Fill in default values until we reach the key index. */
        for (; i < index; ++i)
        {
            g[i].offset = 255;
            g[i].width = 255;
            if (i > 0)
            {
                g[i].location = g[i - 1].location;
            }
            else
            {
                g[0].location = 0;
            }
            /* DebugPrintf("\t{%d, %d, %d} *//* Default *//*\n", g[i].offset,*/
            /*              g[i].width, g[i].location);*/
        }

        g[i].offset = offset;
        g[i].width = width;
        g[i].location = location;
        /*DebugPrintf("\t{%d, %d, %d}\n", g[i].offset, g[i].width,*/
        /*             g[i].location);*/
    }

    return 0;

error:
    return -1;
}

static int loadBitmapSection(struct Font *font, const char *name, FILE *f)
{
    char bitmapSection[33];
    char *section;
    struct Glyph *glyphs = NULL;
    int ret;

    section = getSection(f);
    if (!section)
    {
        goto error;
    }
    snprintf(bitmapSection, sizeof(bitmapSection), "%s:bitmaps", name);
    if (strncmp(section, bitmapSection, 32))
    {
        DebugPrintf("Unexpected section. Wanted '%s'. Got '%s'.\n",
                     bitmapSection, section);
        goto error;
    }
    pfree(section);
    section = NULL;

    if (font->numGlyphs >= 65536)
    {
        /* Too many glyphs */
        goto error;
    }

    glyphs = malloc(font->numGlyphs * sizeof(*font->g));
    if (!glyphs)
    {
        /* Out of memory */
        goto error;
    }

    ret = readBitmapSection(glyphs, font->numGlyphs, f);
    if (ret)
    {
        goto error;
    }

    font->g = glyphs;
    return 0;

error:
    pfree(section);
    free(glyphs);
    return -1;
}

static const struct Font *loadFont(const char *name)
{
    char path[256];
    char *section = NULL;
    FILE *f;
    struct Font *font = NULL;
    int i;
    int ret;
    size_t len;
    struct Rect rect;

    len = strlen(name);
    snprintf(path, sizeof(path), "fonts/%s.ini", name);
    f = fopen(path, "rb");
    if (!f) {
        DebugPrintf("Failed to load %s\n", path);
        return NULL;
    }

    font = malloc(sizeof(*font));
    if (!font)
    {
        DebugPrintf("Out of mem\n");
        goto error;
    }

    font->name = name;
    font->nameLen = len;

    /* Load information for the font */
    consume(f); /* Load first character */
    section = getSection(f);
    if (!section)
    {
        goto error;
    }
    if (strncmp(section, name, 32))
    {
        DebugPrintf("Unexpected section. Wanted '%s'. Got '%s'.\n", name,
                     section);
        goto error;
    }
    pfree(section);


    /* For all valid keys, in any order, read the value. */
    /* Read up to 5 keys. */
    /* We don't yet catch errors involving duplicate or missing keys. */
    for (i = 0; i < 5; i++)
    {
        ret = readKeyPair(font, f);
        if (ret)
        {
            goto error;
        }
    }

    ret = loadBitmapSection(font, name, f);
    if (ret)
    {
        goto error;
    }

    /* Load font bitmap */
    snprintf(path, sizeof(path), "fonts/%s.bmp", name);
    font->img = bmp_load(path, &rect, &font->planes);
    if (!font->img)
    {
        DebugPrintf("Failed to load %s\n", path);
        goto error;
    }

    font->width = rect.right + 1;
    font->height = rect.bottom + 1;

    return font;
error:
    pfree(section);
    free(font);
    fclose(f);
    return NULL;
}

const struct Font *GetFont(const char *name)
{
    size_t len;
    const struct Font *font;

    /* Lazy load the font map. */
    if (!map)
    {
        /*/DebugPrintf("Allocating hashmap\n");*/
        map = hashmap_alloc();
    }
    len = strlen(name);
    font = (struct Font *)hashmap_get(map, name, len);
    if (font)
    {
        /*/DebugPrintf("Previously found font\n");*/
        /* Found previous copy */
        return font;
    }

    /* Didn't find previous copy, so load and add it to the font map. */
    font = loadFont(name);
    if (font)
    {
        /* If the font could be loaded, store in our hashmap for later. */
        hashmap_set(map, font->name, font->nameLen, (ptrdiff_t)font);
    }

    return font;
}

void FreeFont(struct Font *font)
{
    /* Remove the font from the font map. */
    hashmap_remove(map, font->name, font->nameLen);

    /* Free the font's memory */
    if (font)
    {
        free(font->g);
    }

    free(font);
}

int MeasureString(const struct Font *font, const char *s, int *w, int *h)
{
    const struct Glyph *g;
    int x;

    x = 0;
    *h = font->maxHeight; /* Not including leading. */

    for (;*s != '\0'; ++s)
    {
        unsigned int c = *s & 0xFF;
        g = &font->g[c];

        /* Skip unprintable */
        if (g->offset == 255 || g->width == 255)
        {
            continue;
        }

        x += g->width;
    }

    *w = x;

    return 0;
}

int DrawString(const struct Font *font, const char *s, int color, int x, int y)
{
    const struct Glyph *g;
    struct Rect dst;
    struct Rect src;
    int ss;

    if (!s)
    {
        /* No string to draw */
        return 0;
    }

    dst.left = x;
    dst.top = y;
    src.top = 0;
    src.bottom = font->height - 1;
    dst.bottom = dst.top + src.bottom;
    ss = font->width;

    SetColor(color);

    for (;*s != '\0'; ++s)
    {
        unsigned int c = *s & 0xFF;
        g = &font->g[c];

        if (g->offset == 255 || g->width == 255)
        {
            continue;
        }

        /* Draw the glyph */
        src.left = g->location;
        src.right = font->g[c + 1].location
                    - 1; /* TODO could lose all the minus 1 junk if we have it
                            implicit. when a coord used as "right",
                            non-inclusive. when used as a "left", inclusive. */

        dst.right = dst.left + src.right - src.left;
        dst.left += g->offset;
        dst.right += g->offset;
        BlitOp(font->img, OP_SRC_INV, &dst, &src, ss);
        dst.left += g->width - g->offset;
        dst.right += g->width - g->offset;
    }

    return 0;
}
