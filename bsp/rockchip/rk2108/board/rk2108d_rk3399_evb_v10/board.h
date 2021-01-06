/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "board_base.h"

#ifdef USB_VBUS_PIN
#undef USB_VBUS_PIN
#define USB_VBUS_PIN            BANK_PIN(GPIO_BANK1, 17)
#endif

#endif
