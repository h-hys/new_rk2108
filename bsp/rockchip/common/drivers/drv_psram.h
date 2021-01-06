/**
  * Copyright (c) 2020 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_psram.h
  * @version V1.0
  * @brief   psram interface
  *
  * Change Logs:
  * Date           Author          Notes
  * 2020-04-30     Dingqiang Lin   the first version
  *
  ******************************************************************************
  */

#ifndef __DRV_PSRAM_H__
#define __DRV_PSRAM_H__

struct rk_psram_device
{
    struct rt_device parent;

    rt_uint32_t size; /* Byte */

    struct rk_psram_driver_ops *ops;
};

struct rk_psram_driver_ops
{
    rt_size_t (*read)(struct rk_psram_device *device, rt_off_t offset, rt_uint8_t *data, rt_uint32_t length);
    rt_size_t (*write)(struct rk_psram_device *device, rt_off_t offset, const rt_uint8_t *data, rt_uint32_t length);
};

rt_inline rt_size_t rk_psram_read(struct rk_psram_device *dev, rt_off_t pos, rt_uint8_t *data, rt_size_t size)
{
    return dev->ops->read(dev, pos, data, size);
}

rt_inline rt_size_t rk_psram_write(struct rk_psram_device *dev, rt_off_t pos, const rt_uint8_t *data, rt_size_t size)
{
    return dev->ops->write(dev, pos, data, size);
}

#endif
