/*
 *  histgram.h
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2021-05-06.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HISTGRAM_H
#define HISTGRAM_H

#include <stddef.h>

struct Rect;
struct Histogram;

struct Histogram *HistogramAlloc(size_t numBuckets);
void HistogramFree(struct Histogram *h);

void HistogramSetMinValue(struct Histogram *h, int minValue);
void HistogramSetMaxValue(struct Histogram *h, int maxValue);
int HistogramGetMaxSize(const struct Histogram *h);
int HistogramGetMinSize(const struct Histogram *h);

void HistogramPush(struct Histogram *h, int value);
int HistogramGet(const struct Histogram *h, size_t which);

void DrawHistogram(const struct Histogram *h, int color,
                   const struct Rect *rect);

#endif
