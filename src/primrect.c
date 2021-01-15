/*
 *  primrect.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-01-15.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/primrect.h"

static int min(int a, int b)
{
    return a < b ? a : b;
}

static int max(int a, int b)
{
    return a > b ? a : b;
}

void InitRect(struct Rect *rect, int x, int y, int w, int h)
{
    rect->left = x;
    rect->top = y;
    rect->right = x + w - 1;
    rect->bottom = y + h - 1;
}

void RectFromLine(struct Rect *rect, int x0, int y0, int x1, int y1)
{
    /* Create the smallest rectangle that will contain the line segment. */
    rect->left = min(x0, x1);
    rect->top = min(y0, y1);
    rect->right = max(x0, x1);
    rect->bottom = max(y0, y1);
}

static int RectIsEmpty(const struct Rect *rect)
{
    /* XXX Change this when making bottom inclusive */
    if (rect->bottom <= rect->top)
    {
        return 1;
    }

    if (rect->right <= rect->left)
    {
        return 1;
    }

    return 0;
}

int RectUnion(const struct Rect *a, const struct Rect *b, struct Rect *u)
{
    if (RectIsEmpty(a))
    {
        *u = *b;
        return 0;
    }

    if (RectIsEmpty(b))
    {
        *u = *a;
        return 0;
    }

    u->left = min(a->left, b->left);
    u->top = min(a->top, b->top);
    u->right = max(a->right, b->right);
    u->bottom = max(a->bottom, b->bottom);

    return 0;
}

int RectIntersect(const struct Rect *a, const struct Rect *b, struct Rect *i)
{
    /* Fast rejects */
    if (a->bottom < b->top)
    {
        return RECT_NO_INTERSECT;
    }
    if (a->right < b->left)
    {
        return RECT_NO_INTERSECT;
    }
    if (a->top > b->bottom)
    {
        return RECT_NO_INTERSECT;
    }
    if (a->left > b->right)
    {
        return RECT_NO_INTERSECT;
    }

    i->left = max(a->left, b->left);
    i->top = max(a->top, b->top);
    i->right = min(a->right, b->right);
    i->bottom = min(a->bottom, b->bottom);

    return RECT_INTERSECT;
}

void ConvertRect(const struct Rect *rect, int *x, int *y, int *w, int *h)
{
    *x = rect->left;
    *y = rect->top;
    *w = rect->right - rect->left + 1;
    *h = rect->bottom - rect->top + 1;
}

void TranslateRect(struct Rect *rect, int x, int y)
{
    rect->left += x;
    rect->top += y;
    rect->right += x;
    rect->bottom += y;
}

#if 0
int PushClip(struct Rect *rect, const struct Rect *clip)
{
    /* Clip rect to clip */
}

int PopClip(struct Rect *rect, const struct Rect *clip)
{
    /* Restore previously pushed onto clip */
}
#endif

int ClipRect(struct Rect *rect, const struct Rect *clip)
{
    int ret = CLIP_NO_CHANGE;

    /* Fast rejects */
    if (rect->bottom < clip->top)
    {
#define DEBUG 0
#if DEBUG
        SerialPrintf("Too high\n");
#endif
        return CLIP_REJECTED;
    }
    if (rect->right < clip->left)
    {
#if DEBUG
        SerialPrintf("Too far left\n");
#endif
        return CLIP_REJECTED;
    }
    if (rect->top > clip->bottom)
    {
#if DEBUG
        SerialPrintf("Too low\n");
#endif
        return CLIP_REJECTED;
    }
    if (rect->left > clip->right)
    {
#if DEBUG
        SerialPrintf("Too far right\n");
#endif
        return CLIP_REJECTED;
    }

    /* Clips */
    if (rect->top < clip->top)
    {
        rect->top = clip->top;
        ret = CLIP_CLIPPED;
    }
    if (rect->left < clip->left)
    {
        rect->left = clip->left;
        ret = CLIP_CLIPPED;
    }
    if (rect->right > clip->right)
    {
        rect->right = clip->right;
        ret = CLIP_CLIPPED;
    }
    if (rect->bottom > clip->bottom)
    {
        rect->bottom = clip->bottom;
        ret = CLIP_CLIPPED;
    }

    return ret;
#if WE_COULD_USE_INTERSECT
    int ret;

    ret = RectIntersect(rect, clip, rect);
    if (ret == RECT_INTERSECT)
    {
        return CLIP_CLIPPED;
    }

    return CLIP_REJECTED;
#endif
}

int ClipRectAdjust(struct Rect *dst, struct Rect *src, const struct Rect *clip)
{
    int ret;

    /* Clip the dst rect based on the clipping rect. */
    /* Adjust the src rect based on how the dst rect was clipped. */

    struct Rect orig;

    /* Adjust dst clipping */
    orig = *dst;
    ret = ClipRect(dst, clip);
    if (ret == CLIP_REJECTED)
    {
        return CLIP_REJECTED;
    }

    /* Adjust src based on dst clipping */
    src->left += dst->left - orig.left; /* new_val - orig_val */
    src->top += dst->top - orig.top; /* new_val - orig_val */
    src->right -= (orig.right - dst->right);
    src->bottom -= (orig.bottom - dst->bottom);

    return CLIP_CLIPPED;
}
