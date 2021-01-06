/**
  * Copyright (c) 2018 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    interrupt.c
  * @version V0.1
  * @brief   interrupt interface for rt-thread
  *
  * Change Logs:
  * Date           Author          Notes
  * 2019-05-08     Cliff.Chen      first implementation
  *
  ******************************************************************************
  */

#include <rthw.h>
#include "hal_base.h"

#if defined(ARCH_ARM_CORTEX_M0) || defined(ARCH_ARM_CORTEX_M3) || defined(ARCH_ARM_CORTEX_M4) || defined(ARCH_ARM_CORTEX_M7)

void rt_hw_interrupt_init(void)
{
    uint32_t i;

    for (i = 0; i < NUM_INTERRUPTS; i++)
    {
        HAL_NVIC_SetPriority(i, NVIC_PERIPH_PRIO_DEFAULT, NVIC_PERIPH_SUB_PRIO_DEFAULT);
    }
}

void rt_hw_interrupt_mask(int vector)
{
    HAL_NVIC_DisableIRQ(vector);
}

void rt_hw_interrupt_umask(int vector)
{
    HAL_NVIC_EnableIRQ(vector);
}

rt_isr_handler_t rt_hw_interrupt_install(int              vector,
        rt_isr_handler_t handler,
        void            *param,
        const char      *name)
{
    HAL_NVIC_SetIRQHandler(vector, (NVIC_IRQHandler)handler);
    return handler;
}

#endif
