/**
  * Copyright (c) 2019 Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    board.h
  * @author  Jason Zhu
  * @version V0.1
  * @date    1-Aug-2019
  * @brief
  *
  ******************************************************************************
  */

#ifndef __BOARD__
#define __BOARD__

#include "drv_uart.h"

extern  void *_end;
extern  void *_heap_end;
#define HEAP_BEGIN  &_end
#define HEAP_END    &_heap_end

#ifdef RT_USING_CAMERA
#define CAMERA_PWDN_GPIO_BANK               GPIO_BANK1
#define CAMERA_PWDN_GPIO_GROUP              GPIO1
#define CAMERA_PWDN_GPIO_PIN                GPIO_PIN_D4

#define CAMERA_RST_GPIO_BANK                GPIO_BANK1
#define CAMERA_RST_GPIO_GROUP               GPIO1
#define CAMERA_RST_GPIO_PIN                 GPIO_PIN_D5

#define CAMERA_MCLK_GPIO_BANK               GPIO_BANK2
#define CAMERA_MCLK_GPIO_GROUP              GPIO2
#define CAMERA_MCLK_GPIO_PIN                GPIO_PIN_A3

#define CAMERA_PW_EN_BANK                   GPIO_BANK3
#define CAMERA_PW_EN_GROUP                  GPIO3
#define CAMERA_PW_EN_GPIO_PIN               GPIO_PIN_A6
#endif

#endif
