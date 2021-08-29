/*
 *  hash.h
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-04.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HASH_H
#define HASH_H

#include "guikit/ptypes.h"

#include <stddef.h>

unsigned long hash(const char *buf, size_t len);

void hash64(u32 out[], const char *buf, size_t len);

#endif
