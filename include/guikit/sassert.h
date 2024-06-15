/**
 *  @file sassert.h
 *  @brief Static Assert
 *
 *  Patater GUI Kit
 *  Created by Jaeden Amero on 2019-05-05.
 *  Copyright 2019. SPDX-License-Identifier: AGPL-3.0-or-later
 */

#ifndef SASSERT_H
#define SASSERT_H

#define STATIC_ASSERT(x, msg) \
    extern int STATIC_ASSERT_##msg[1]; \
    extern int STATIC_ASSERT_##msg[(x)?1:2]

#endif
