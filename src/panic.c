/*
 *  panic.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2019-05-04.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/panic.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/* LCOV_EXCL_START */
void panic(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    exit(EXIT_FAILURE);
}
/* LCOV_EXCL_STOP */
