/*
 *  snprintf.h
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2020-12-29.
 *  Copyright 2020. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef SNPRINTF_H
#define SNPRINTF_H

#include <stddef.h>
#include <stdarg.h>

int vsnprintf(char *str, size_t size, const char *format, va_list ap);

int snprintf(char *str, size_t size, const char *format, ...);

#endif
