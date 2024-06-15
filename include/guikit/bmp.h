/**
 *  @file bmp.h
 *  @brief BMP
 *
 *  Patater GUI Kit
 *  Created by Jaeden Amero on 2020-12-24.
 *  Copyright 2020. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef BMP_H
#define BMP_H

struct Rect;

unsigned char *bmp_load(const char *path, struct Rect *rect, short *planes);
void bmp_free(unsigned char *img);

#endif
