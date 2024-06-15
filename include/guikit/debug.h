/**
 *  @file debug.h
 *  @brief Debug
 *
 *  Patater GUI Kit
 *  Created by Jaeden Amero on 2020-12-20.
 *  Copyright 2020. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdarg.h>

int DebugInit(void);
void DebugFree(void);

int DebugPoll(void);

int DebugPutchar(char c);
int DebugPuts(const char *str);
int DebugVPrintf(const char *format, va_list ap);
int DebugPrintf(const char *format, ...);

#endif
