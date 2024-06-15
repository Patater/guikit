/**
 *  @file prandom.h
 *  @brief Patater Random
 *
 *  Patater GUI Kit
 *  Created by Jaeden Amero on 2021-05-03.
 *  Copyright 2021. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/ptypes.h"

/* Return a pseudo-random 32-bit unsigned integer. */
u32 prandom(void);

/* Return a pseudo-random 32-bit unsigned integer within the given range. */
u32 RandRange(u32 smallest, u32 biggest);
