/*
 *  snprintf.c
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2020-12-29.
 *  Copyright 2020. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "guikit/snprintf.h"
#include "guikit/serial.h"

#include <stdio.h>
#include <string.h>

/*
This function works similarly to sprintf() (see section sprintf), but the size
n of the buffer is also taken into account. This function will write n - 1
characters. The nth character is used for the terminating nul. If n is zero,
buffer is not touched.

Return Value

The number of characters that would have been written (excluding the trailing
nul) is returned; otherwise -1 is returned to flag encoding or buffer space
errors.

The maximum accepted value of n is INT_MAX. INT_MAX is defined in <limits.h>.
-1 is returned and errno is set to EFBIG, if n is greater than this limit.
*/

static int min(int a, int b)
{
    return a < b ? a : b;
}

int vsnprintf(char *str, size_t size, const char *format,
              va_list ap)
{
    static char tmp[4096]; /* Big-sized buffer. May not be big enough. */
    int num_chars_written;
    int num_chars_copied;

    if (size == 0)
    {
        return 0;
    }

    /* Cop out implementation */
    if (size >= sizeof(tmp))
    {
        /* TODO Set errno to EFBIG. */
        return -1;
    }
    num_chars_written = vsprintf(tmp, format, ap);
    num_chars_copied = min(num_chars_written, size - 1);
    memcpy(str, tmp, num_chars_copied);
    str[num_chars_copied] = '\0';

    return num_chars_copied;
}

int snprintf(char *str, size_t size, const char *format,
             ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vsnprintf(str, size, format, ap);
    va_end(ap);
    return ret;
}
