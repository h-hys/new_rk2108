/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __TOUCHPAD_H__
#define __TOUCHPAD_H__
#include <rtthread.h>

#include "touch.h"

struct rt_touchpad_data
{
    rt_device_t dev;
    rt_thread_t thread;
    rt_sem_t    sem;

    rt_uint8_t    id[8];
    struct rt_touch_info info;
    struct rt_touch_data *point;

    void (*callback)(struct rt_touch_data *p, rt_uint8_t num);
};

/**
 * touchpad init.
 */
int rt_touchpad_thread_init(const char *name,
                            void (*callback)(struct rt_touch_data *p, rt_uint8_t num));
/**
 * touchpad deinit.
 */
void rt_touchpad_thread_deinit(void);

#endif
