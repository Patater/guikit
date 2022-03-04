/*
 *  fps.h
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2022-03-05.
 *  Copyright 2022. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef FPS_H
#define FPS_H

struct Font;

void InitFPSTimer(void);
void DisplayFPS(const struct Font *font, int textHeight);

#endif
