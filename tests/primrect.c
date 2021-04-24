/*
 *  primrect.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-01-15.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/primrect.h"
#include "test.h"

static int rectangleIsClippedNW(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 15;
    rect.top = 15;
    rect.right = 25;
    rect.bottom = 25;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_CLIPPED);
    TEST_EQ(rect.left, 20);
    TEST_EQ(rect.top, 20);
    TEST_EQ(rect.right, 25);
    TEST_EQ(rect.bottom, 25);

    return 0;
}

static int rectangleIsClippedN(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 20;
    rect.top = 15;
    rect.right = 30;
    rect.bottom = 25;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_CLIPPED);
    TEST_EQ(rect.left, 20);
    TEST_EQ(rect.top, 20);
    TEST_EQ(rect.right, 30);
    TEST_EQ(rect.bottom, 25);

    return 0;
}

static int rectangleIsClippedNE(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 25;
    rect.top = 15;
    rect.right = 35;
    rect.bottom = 25;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_CLIPPED);
    TEST_EQ(rect.left, 25);
    TEST_EQ(rect.top, 20);
    TEST_EQ(rect.right, 30);
    TEST_EQ(rect.bottom, 25);

    return 0;
}

static int rectangleIsClippedE(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 25;
    rect.top = 20;
    rect.right = 35;
    rect.bottom = 30;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_CLIPPED);
    TEST_EQ(rect.left, 25);
    TEST_EQ(rect.top, 20);
    TEST_EQ(rect.right, 30);
    TEST_EQ(rect.bottom, 30);

    return 0;
}

static int rectangleIsClippedSE(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 25;
    rect.top = 25;
    rect.right = 35;
    rect.bottom = 35;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_CLIPPED);
    TEST_EQ(rect.left, 25);
    TEST_EQ(rect.top, 25);
    TEST_EQ(rect.right, 30);
    TEST_EQ(rect.bottom, 30);

    return 0;
}

static int rectangleIsClippedS(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 20;
    rect.top = 25;
    rect.right = 30;
    rect.bottom = 80;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_CLIPPED);
    TEST_EQ(rect.left, 20);
    TEST_EQ(rect.top, 25);
    TEST_EQ(rect.right, 30);
    TEST_EQ(rect.bottom, 30);

    return 0;
}

static int rectangleIsClippedSW(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 15;
    rect.top = 25;
    rect.right = 25;
    rect.bottom = 35;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_CLIPPED);
    TEST_EQ(rect.left, 20);
    TEST_EQ(rect.top, 25);
    TEST_EQ(rect.right, 25);
    TEST_EQ(rect.bottom, 30);

    return 0;
}

static int rectangleIsClippedW(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 15;
    rect.top = 20;
    rect.right = 25;
    rect.bottom = 30;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_CLIPPED);
    TEST_EQ(rect.left, 20);
    TEST_EQ(rect.top, 20);
    TEST_EQ(rect.right, 25);
    TEST_EQ(rect.bottom, 30);

    return 0;
}

static int rectangleIsRejectedNW(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 15;
    rect.top = 15;
    rect.right = 19;
    rect.bottom = 19;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_REJECTED);

    return 0;
}

static int rectangleIsRejectedN(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 20;
    rect.top = 15;
    rect.right = 30;
    rect.bottom = 19;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_REJECTED);

    return 0;
}

static int rectangleIsRejectedNE(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 30;
    rect.top = 15;
    rect.right = 35;
    rect.bottom = 19;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_REJECTED);

    return 0;
}

static int rectangleIsRejectedE(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 31;
    rect.top = 20;
    rect.right = 35;
    rect.bottom = 30;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_REJECTED);

    return 0;
}

static int rectangleIsRejectedSE(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 31;
    rect.top = 30;
    rect.right = 35;
    rect.bottom = 35;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_REJECTED);

    return 0;
}

static int rectangleIsRejectedS(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 20;
    rect.top = 31;
    rect.right = 30;
    rect.bottom = 35;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_REJECTED);

    return 0;
}

static int rectangleIsRejectedSW(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 15;
    rect.top = 30;
    rect.right = 19;
    rect.bottom = 35;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_REJECTED);

    return 0;
}

static int rectangleIsRejectedW(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 15;
    rect.top = 20;
    rect.right = 19;
    rect.bottom = 29;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_REJECTED);

    return 0;
}

static int rectangleIsNoChange(void)
{
    struct Rect rect;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    rect.left = 20;
    rect.top = 20;
    rect.right = 29;
    rect.bottom = 29;

    ret = ClipRect(&rect, &clip);

    TEST_EQ(ret, CLIP_NO_CHANGE);

    return 0;
}

static int rectangleAdjustClipS(void)
{
    struct Rect dst;
    struct Rect src;
    struct Rect clip;
    int ret;

    clip.left = 20;
    clip.top = 20;
    clip.right = 30;
    clip.bottom = 30;

    dst.left = 25;
    dst.top = 25;
    dst.right = 35;
    dst.bottom = 35;

    src.left = 0;
    src.top = 0;
    src.right = 10;
    src.bottom = 10;

    ret = ClipRectAdjust(&dst, &src, &clip);

    TEST_EQ(ret, CLIP_CLIPPED);

    TEST_EQ(dst.left, 25);
    TEST_EQ(dst.top, 25);
    TEST_EQ(dst.right, 30);
    TEST_EQ(dst.bottom, 30);

    TEST_EQ(src.left, 0);
    TEST_EQ(src.top, 0);
    TEST_EQ(src.right, 5);
    TEST_EQ(src.bottom, 5);

    return 0;
}

int rectUnionOverlap(void)
{
    struct Rect a;
    struct Rect b;
    struct Rect u;
    int ret;

    a.left = 0;
    a.top = 0;
    a.right = 10;
    a.bottom = 10;

    b.left = 5;
    b.top = 5;
    b.right = 15;
    b.bottom = 15;

    u.left = -1;
    u.top = -1;
    u.right = -1;
    u.bottom = -1;

    ret = RectUnion(&a, &b, &u);

    TEST_EQ(ret, 0);
    TEST_EQ(u.left, 0);
    TEST_EQ(u.top, 0);
    TEST_EQ(u.right, 15);
    TEST_EQ(u.bottom, 15);

    return 0;
}

int rectUnionEmptyIsSelf(void)
{
    struct Rect self;
    struct Rect empty;
    struct Rect u;
    int ret;

    self.left = 10;
    self.top = 20;
    self.right = 30;
    self.bottom = 40;

    empty.left = 0;
    empty.top = 0;
    empty.right = 0;
    empty.bottom = 0;

    u.left = -1;
    u.top = -1;
    u.right = -1;
    u.bottom = -1;

    ret = RectUnion(&empty, &self, &u);

    TEST_EQ(ret, 0);
    TEST_EQ(u.left, self.left);
    TEST_EQ(u.top, self.top);
    TEST_EQ(u.right, self.right);
    TEST_EQ(u.bottom, self.bottom);

    return 0;
}

int rectUnionWithEmptyFirstIsSelf(void)
{
    struct Rect self;
    struct Rect empty;
    int ret;

    self.left = 10;
    self.top = 20;
    self.right = 30;
    self.bottom = 40;

    empty.left = 0;
    empty.top = 0;
    empty.right = 0;
    empty.bottom = 0;

    ret = RectUnion(&empty, &self, &self);

    TEST_EQ(ret, 0);
    TEST_EQ(self.left, 10);
    TEST_EQ(self.top, 20);
    TEST_EQ(self.right, 30);
    TEST_EQ(self.bottom, 40);

    return 0;
}

int rectUnionWithEmptySecondIsSelf(void)
{
    struct Rect self;
    struct Rect empty;
    int ret;

    self.left = 10;
    self.top = 20;
    self.right = 30;
    self.bottom = 40;

    empty.left = 0;
    empty.top = 0;
    empty.right = 0;
    empty.bottom = 0;

    ret = RectUnion(&self, &empty, &self);

    TEST_EQ(ret, 0);
    TEST_EQ(self.left, 10);
    TEST_EQ(self.top, 20);
    TEST_EQ(self.right, 30);
    TEST_EQ(self.bottom, 40);

    return 0;
}

int rectIntersectOverlap(void)
{
    struct Rect a;
    struct Rect b;
    struct Rect i;
    int ret;

    a.left = 0;
    a.top = 0;
    a.right = 10;
    a.bottom = 10;

    b.left = 5;
    b.top = 5;
    b.right = 15;
    b.bottom = 15;

    i.left = -1;
    i.top = -1;
    i.right = -1;
    i.bottom = -1;

    ret = RectIntersect(&a, &b, &i);

    TEST_EQ(ret, RECT_INTERSECT);
    TEST_EQ(i.left, 5);
    TEST_EQ(i.top, 5);
    TEST_EQ(i.right, 10);
    TEST_EQ(i.bottom, 10);

    return 0;
}

int rectIntersectNoOverlap(void)
{
    struct Rect a;
    struct Rect b;
    struct Rect i;
    int ret;

    a.left = 0;
    a.top = 0;
    a.right = 10;
    a.bottom = 10;

    b.left = 15;
    b.top = 15;
    b.right = 25;
    b.bottom = 25;

    i.left = -1;
    i.top = -1;
    i.right = -1;
    i.bottom = -1;

    ret = RectIntersect(&a, &b, &i);

    TEST_EQ(ret, RECT_NO_INTERSECT);

    return 0;
}

int rectNormalizedReverseX(void)
{
    struct Rect r;
    r.left = 441;
    r.top = 242;
    r.right = 378;
    r.bottom = 266;

    NormalizedRect(&r, &r);

    TEST_EQ(r.left, 378);
    TEST_EQ(r.top, 242);
    TEST_EQ(r.right, 441);
    TEST_EQ(r.bottom, 266);

    return 0;
}

int rectNormalizedForwardX(void)
{
    struct Rect r;
    r.left = 378;
    r.top = 242;
    r.right = 441;
    r.bottom = 266;

    NormalizedRect(&r, &r);

    TEST_EQ(r.left, 378);
    TEST_EQ(r.top, 242);
    TEST_EQ(r.right, 441);
    TEST_EQ(r.bottom, 266);

    return 0;
}

int rectNormalizedReverseY(void)
{
    struct Rect r;
    r.left = 378;
    r.top = 266;
    r.right = 441;
    r.bottom = 242;

    NormalizedRect(&r, &r);

    TEST_EQ(r.left, 378);
    TEST_EQ(r.top, 242);
    TEST_EQ(r.right, 441);
    TEST_EQ(r.bottom, 266);

    return 0;
}

int rectNormalizedForwardY(void)
{
    struct Rect r;
    r.left = 378;
    r.top = 242;
    r.right = 441;
    r.bottom = 266;

    NormalizedRect(&r, &r);

    TEST_EQ(r.left, 378);
    TEST_EQ(r.top, 242);
    TEST_EQ(r.right, 441);
    TEST_EQ(r.bottom, 266);

    return 0;
}

const test_fn tests[] =
{
    rectangleIsClippedNW,

    rectangleIsClippedN,
    rectangleIsClippedNE,
    rectangleIsClippedE,
    rectangleIsClippedSE,
    rectangleIsClippedS,
    rectangleIsClippedSW,
    rectangleIsClippedW,

    rectangleIsRejectedNW,
    rectangleIsRejectedN,
    rectangleIsRejectedNE,
    rectangleIsRejectedE,
    rectangleIsRejectedSE,
    rectangleIsRejectedS,
    rectangleIsRejectedSW,
    rectangleIsRejectedW,

    rectangleIsNoChange,

    rectangleAdjustClipS,

    rectUnionOverlap,
    rectUnionWithEmptySecondIsSelf,
    rectUnionWithEmptyFirstIsSelf,
    rectIntersectOverlap,
    rectIntersectNoOverlap,

    rectNormalizedReverseX,
    rectNormalizedForwardX,
    rectNormalizedReverseY,
    rectNormalizedForwardY,

    0
};
