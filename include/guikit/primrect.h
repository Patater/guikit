/*
 *  primrect.h
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-01-15.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef PRIMRECT_H
#define PRIMRECT_H

/* Primitive rectangle */

struct Rect {
    int left;
    int top;
    int right; /* x + w - 1 */
    int bottom; /* y + h - 1 */
};

enum {
    CLIP_NO_CHANGE = 0,
    CLIP_CLIPPED,
    CLIP_REJECTED,
    NUM_CLIP
};

enum {
    RECT_NO_INTERSECT = 0,
    RECT_INTERSECT,
    NUM_RECT_INTERSECTS
};

void InitRect(struct Rect *rect, int x, int y, int w, int h);
void RectFromLine(struct Rect *rect, int x0, int y0, int x1, int y1);
void ConvertRect(const struct Rect *rect, int *x, int *y, int *w, int *h);

int RectUnion(const struct Rect *a, const struct Rect *b,
              struct Rect *u);
int RectIntersect(const struct Rect *a, const struct Rect *b,
                  struct Rect *i);

/* Transformatons and Motion */
void TranslateRect(struct Rect *rect, int x, int y);
int ClipRect(struct Rect *rect, const struct Rect *clip);
int ClipRectAdjust(struct Rect *dst, struct Rect *src,
                   const struct Rect *clip);

#endif
