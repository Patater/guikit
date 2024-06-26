/**
 *  @file hashmap.h
 *  @brief Hashmap
 *
 *  Patater GUI Kit
 *  Created by Jaeden Amero on 2019-05-04.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef HASHMAP_H
#define HASHMAP_H

#include "guikit/ptypes.h"

#include <stddef.h>

struct hashmap;

struct hashmap *hashmap_alloc(void);
void hashmap_free(struct hashmap *hashmap);

/* Note that our hashmap can't handle zero as a hash value, as it uses 0 to
 * know if a value is present or not in the keys array. */
ptrdiff_t
hashmap_get(const struct hashmap *hashmap,
            const u32 hash[]);

void hashmap_put(struct hashmap *hashmap,
                 const u32 hash[], ptrdiff_t value);

void hashmap_del(struct hashmap *hashmap,
                 const u32 hash[]);

size_t hashmap_capacity(const struct hashmap *hashmap);

size_t hashmap_length(const struct hashmap *hashmap);

size_t hashmap_num_collisions(const struct hashmap *hashmap);

#endif
