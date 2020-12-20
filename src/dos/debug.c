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

#include "guikit/ansidos.h"
#include <dos.h>
#include <pc.h>
#include <dpmi.h>

enum
{
    COM1 = 0,
    COM2 = 1,
    COM3 = 2,
    COM4 = 3
};

int DebugInit(void)
{
    union REGS r;

    /* We use BIOS calls in hopes of better compatibility with more DPMI hosts.
     * */

    r.x.dx = COM4;
    r.h.al = 0xE3; /* 9600 8-N-1 */
    r.h.ah = 0x00; /* Serial port init */
    int86(0x14, &r, &r);

    return 0;
}

void DebugFree(void)
{
    /* TODO When we register a custom interrupt handler, we should unregister
     * it here. */
    return;
}

int DebugPutchar(char c)
{
    union REGS r;

    /* Not sure if this waits for transmit holding register empty or not... */

    r.x.dx = COM4;
    r.h.al = c;
    r.h.ah = 0x01; /* Serial port write */
    int86(0x14, &r, &r);

    return r.h.ah & 0x7; /* Bit 7 clear if success */
}

int DebugPuts(const char *str)
{
    while (*str != '\0')
    {
        DebugPutchar(*str);

        /* Auto-CR */
        if (*str == '\n')
        {
            DebugPutchar('\r');
        }

        ++str;
    }

    return 0;
}

static int serialIsDataReady()
{
    union REGS r;

    r.x.dx = COM4;
    r.x.ax = 0x03; /* Query status */
    int86(0x14, &r, &r);
    return r.h.ah & 0x01;
}

static int serialReadData()
{
    union REGS r;

    r.x.dx = COM4;
    r.x.ax = 0x01; /* Serial port read */
    int86(0x14, &r, &r);

    return r.h.ah;
}

/* NOTE For serial input, we probably want an interrupt handler to gather data
 * at port as soon as it comes into the UART FIFO, rather than polling the
 * port. */
int DebugPoll()
{
    char tmp[32]; /* Big enough to hold a 16-byte FIFO's worth of data with
                     room to spare. */
    int i = 0;

    while (serialIsDataReady())
    {
        tmp[i++] = serialReadData();
    }

    (void)tmp;
    return 0;
}

int DebugVPrintf(const char *format, va_list ap)
{
    static char tmp[4096]; /* Big-sized buffer. May not be big enough. */
    int num_chars_written;

    /* Re-use sprintf. Quick and dirty. */
    num_chars_written = vsprintf(tmp, format, ap);
    DebugPuts(tmp);

    return num_chars_written;
}

int DebugPrintf(const char *format, ...)
{
    int num_chars_written;
    va_list ap;
    va_start(ap, format);

    num_chars_written = DebugVPrintf(format, ap);

    va_end(ap);

    return num_chars_written;
}
