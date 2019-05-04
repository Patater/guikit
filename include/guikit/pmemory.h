/*
 *  pmemory.h
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-04.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef PMEMORY_H
#define PMEMORY_H

#include <stddef.h>

void *pmalloc(size_t size);
void *pcalloc(size_t number, size_t size);
void *prealloc(void *ptr, size_t size);
void pfree(void *ptr);

#endif
