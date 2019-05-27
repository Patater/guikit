/*
 *  ptypes.h
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-27.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef PTYPES_H
#define PTYPES_H

#include "guikit/sassert.h"

typedef unsigned char u8;
STATIC_ASSERT(sizeof(u8) == 1, incorrect_u8_size);

typedef unsigned short u16;
STATIC_ASSERT(sizeof(u16) == 2, incorrect_u16_size);

typedef unsigned int u32;
STATIC_ASSERT(sizeof(u32) == 4, incorrect_u32_size);

typedef signed char i8;
STATIC_ASSERT(sizeof(i8) == 1, incorrect_i8_size);

typedef signed short i16;
STATIC_ASSERT(sizeof(i16) == 2, incorrect_i16_size);

typedef signed int i32;
STATIC_ASSERT(sizeof(i32) == 4, incorrect_i32_size);

#endif
