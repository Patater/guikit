/*
 *  ansidos.h
 *  Patater GUI Kit
 *
 *  Created by Jaeden Amero on 2020-12-12.
 *  Copyright 2020. SPDX-License-Identifier: AGPL-3.0-or-later
 */

/* ANSI with DOS stuff allowed. Include this file before including any DJGPP
 * headers. */

/* If we specify the ANSI C standard with '-ansi', the DJGPP headers willl have
 * __STRICT_ANSI__ set and not add definitions for things like dos.h, dpmi.h,
 * and pc.h. We don't like that behavior, so we need to undefine
 * __STRICT_ANSI__ temporarily for DJGPP headers. */

#ifndef ANSI_DOS_H
#define ANSI_DOS_H

#ifdef __DJGPP__

#undef __STRICT_ANSI__

#endif

#endif
