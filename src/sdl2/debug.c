/*
 *  debug.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2020-12-20.
 *  Copyright 2020. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/debug.h"

#include <stdarg.h>
#include <stdio.h>

int DebugInit(void)
{
    return 0;
}

void DebugFree(void)
{
    return;
}

int DebugPutchar(char c)
{
    putchar(c);
    return 0;
}

int DebugPuts(const char *str)
{
    puts(str);

    return 0;
}

int DebugPoll(void)
{
    return 0;
}

int DebugVPrintf(const char *format, va_list ap)
{
    return vprintf(format, ap);
}

int DebugPrintf(const char *format, ...)
{
    int num_chars_written;
    va_list ap;
    va_start(ap, format);

    num_chars_written = vprintf(format, ap);

    va_end(ap);

    return num_chars_written;
}
