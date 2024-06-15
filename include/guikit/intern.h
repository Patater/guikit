/**
 *  @file intern.h
 *  @brief Intern
 *
 *  Patater GUI Kit
 *  Created by Jaeden Amero on 2019-05-06.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef INTERN_H
#define INTERN_H

/* Release all interned strings and reset statistics counters. */
void intern_free(void);

const char *intern(const char *s);
int intern_num_hits(void);
int intern_num_interned(void);

#endif
