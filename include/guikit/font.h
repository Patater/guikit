/**
 *  @file font.h
 *  @brief Font
 *
 *  Patater GUI Kit
 *  Created by Jaeden Amero on 2020-12-29.
 *  Copyright 2020. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef FONT_H
#define FONT_H

struct Font;

const struct Font *GetFont(const char *name);
void FreeFont(struct Font *font);

int MeasureString(const struct Font *font, const char *s, int *w, int *h);

int DrawString(const struct Font *font, const char *s, int x, int y);


#if 0
int DrawPrintf();
int DrawChar(); /* Don't use for strings, as it doesn't do any kerning or
justification or other nice stuff. */
#endif

#endif
