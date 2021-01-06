/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_snor.h
  * @version V1.0
  * @brief   spi nor driver headfile
  *
  * Change Logs:
  * Date           Author          Notes
  * 2019-02-20     Dingqiang Lin   the first version
  * 2019-06-27     Dingqiang Lin   support FSPI
  *
  ******************************************************************************
  */

#ifndef __DRV_SFC_NOR_MTD_H__
#define __DRV_SFC_NOR_MTD_H__

rt_base_t snor_xip_suspend(void);
void snor_xip_resume(rt_base_t level);
rt_err_t snor_read_uuid(uint8_t *buf);

#endif
