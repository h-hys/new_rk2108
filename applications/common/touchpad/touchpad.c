/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtthread.h>

#include "touch.h"
#include "touchpad.h"

static  struct rt_touchpad_data *g_tp_data = RT_NULL;

static rt_err_t rx_callback(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(g_tp_data->sem);
    rt_device_control(dev, RT_TOUCH_CTRL_DISABLE_INT, RT_NULL);
    return 0;
}

static void rt_touchpad_thread(void *parameter)
{
    struct rt_touch_data *read_data = g_tp_data->point;
    rt_uint8_t point_max = g_tp_data->info.point_num;
    rt_uint8_t point_num = 0;

    while (1)
    {
        rt_sem_take(g_tp_data->sem, RT_WAITING_FOREVER);

        point_num = rt_device_read(g_tp_data->dev, 0, read_data, point_max);
        if (point_num)
        {
#if 0
            for (rt_uint8_t i = 0; i < point_max; i++)
            {
                if (read_data[i].event != RT_TOUCH_EVENT_NONE)
                {
                    rt_kprintf("%d %d %d %d %d %d\n",
                               read_data[i].track_id,
                               read_data[i].event,
                               read_data[i].x_coordinate,
                               read_data[i].y_coordinate,
                               read_data[i].timestamp,
                               read_data[i].width);
                }
            }
#endif

            if (g_tp_data->callback)
            {
                g_tp_data->callback(read_data, point_num);
            }
        }
        rt_device_control(g_tp_data->dev, RT_TOUCH_CTRL_ENABLE_INT, RT_NULL);
    }
}

/**
 * touchpad init.
 */
int rt_touchpad_thread_init(const char *name,
                            void (*callback)(struct rt_touch_data *p, rt_uint8_t num))
{
    rt_err_t ret;

    g_tp_data = (struct rt_touchpad_data *)rt_malloc(sizeof(struct rt_touchpad_data));
    RT_ASSERT(g_tp_data != RT_NULL);
    rt_memset((void *)g_tp_data, 0, sizeof(struct rt_touchpad_data));

    g_tp_data->dev = rt_device_find(name);
    RT_ASSERT(g_tp_data->dev != RT_NULL);

    ret = rt_device_open(g_tp_data->dev, RT_DEVICE_FLAG_INT_RX);
    RT_ASSERT(ret == RT_EOK);

    rt_device_control(g_tp_data->dev, RT_TOUCH_CTRL_GET_ID, g_tp_data->id);
    rt_kprintf("id = gt%s\n", g_tp_data->id);

    rt_device_control(g_tp_data->dev, RT_TOUCH_CTRL_GET_INFO, &g_tp_data->info);
    rt_kprintf("x_range = %d\ny_range = %d\nmax_point = %d\n", g_tp_data->info.range_x, g_tp_data->info.range_y, g_tp_data->info.point_num);

    g_tp_data->point = (struct rt_touch_data *)rt_malloc(sizeof(struct rt_touch_data) * g_tp_data->info.point_num);
    RT_ASSERT(g_tp_data->point != RT_NULL);
    rt_memset(g_tp_data->point, 0, sizeof(struct rt_touch_data) * g_tp_data->info.point_num);

    rt_device_set_rx_indicate(g_tp_data->dev, rx_callback);

    g_tp_data->sem = rt_sem_create("tpsem", 0, RT_IPC_FLAG_FIFO);
    RT_ASSERT(g_tp_data->sem != RT_NULL);

    g_tp_data->callback = callback;

    g_tp_data->thread = rt_thread_create("touchpad", rt_touchpad_thread, RT_NULL, 1024, 5, 10);
    RT_ASSERT(g_tp_data->thread != RT_NULL);
    rt_thread_startup(g_tp_data->thread);

    return RT_EOK;
}

/**
 * touchpad deinit.
 */
void rt_touchpad_thread_deinit(void)
{
    rt_err_t ret;

    rt_thread_delete(g_tp_data->thread);
    rt_thread_delay(10);

    ret = rt_sem_delete(g_tp_data->sem);
    RT_ASSERT(ret == RT_EOK);

    ret = rt_device_close(g_tp_data->dev);
    RT_ASSERT(ret == RT_EOK);

    rt_free(g_tp_data->point);
    rt_free(g_tp_data);
}
