/**
  * Copyright (c) 2019 Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    board.c
  * @author  Jason Zhu
  * @version V0.1
  * @date    1-Aug-2019
  * @brief
  *
  ******************************************************************************
  */

#include <rtthread.h>
#include <rthw.h>
#include "board.h"
#include "drv_heap.h"
#include "hal_base.h"
#include "hal_bsp.h"
#include "timer.h"
#include "../cpu/riscv_csr_encoding.h"

#define WRITE_REG(REG, VAL)  ((*(volatile uint32_t *)&(REG)) = (VAL))

#if defined(RT_USING_UART0)
RT_WEAK const struct uart_board g_uart0_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart0",
};
#endif /* RT_USING_UART0 */

#ifdef RT_USING_USB_DEVICE
#include "drv_usbd.h"

struct ep_id g_usb_ep_pool[] =
{
    { 0x0,  USB_EP_ATTR_CONTROL,    USB_DIR_INOUT,  64,   ID_ASSIGNED   },
    { 0x1,  USB_EP_ATTR_BULK,       USB_DIR_IN,     512,  ID_UNASSIGNED },
    { 0x2,  USB_EP_ATTR_BULK,       USB_DIR_OUT,    512,  ID_UNASSIGNED },
    { 0x3,  USB_EP_ATTR_BULK,       USB_DIR_IN,     512,  ID_UNASSIGNED },
    { 0x4,  USB_EP_ATTR_BULK,       USB_DIR_OUT,    512,  ID_UNASSIGNED },
    { 0x5,  USB_EP_ATTR_BULK,       USB_DIR_IN,     512,  ID_UNASSIGNED },
    { 0x6,  USB_EP_ATTR_BULK,       USB_DIR_OUT,    512,  ID_UNASSIGNED },
    { 0x7,  USB_EP_ATTR_INT,        USB_DIR_IN,     64,   ID_UNASSIGNED },
    { 0x8,  USB_EP_ATTR_INT,        USB_DIR_IN,     64,   ID_UNASSIGNED },
    { 0x9,  USB_EP_ATTR_INT,        USB_DIR_IN,     64,   ID_UNASSIGNED },
    { 0xFF, USB_EP_ATTR_TYPE_MASK,  USB_DIR_MASK,   0,    ID_ASSIGNED   },
};
#endif

int rt_hw_board_init(void)
{
    HAL_Init();

    rt_system_heap_init((void *)HEAP_BEGIN, (void *)HEAP_END);
    rt_hw_interrupt_init();
#ifdef RT_USING_UART
    rt_hw_usart_init();
#endif
#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
    sysTick_config(SCR1_CORE_FREQUECY / RT_TICK_PER_SECOND);

    rt_components_board_init();

    return 0;
}
