/*
 * Copyright (c) 2020 Fuzhou Rockchip Electronic Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-04     Jair Wu      First version
 *
 */

#ifndef __SETTING_PAGE_H__
#define __SETTING_PAGE_H__

#include "recpen.h"

#define SETTING_PER_PAGE        8

void setting_page_init(int p_page);
void setting_page_deinit(void);
void setting_page_key(size_t type, size_t value);

#endif
