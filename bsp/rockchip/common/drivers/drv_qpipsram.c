/**
  * Copyright (c) 2020 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_qpipsram.c
  * @version V1.0
  * @brief   qpi psram interface
  *
  * Change Logs:
  * Date           Author          Notes
  * 2020-04-30     Dingqiang Lin   the first version
  *
  ******************************************************************************
  */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup QPIPSRAM
 *  @{
 */

/** @defgroup QPIPSRAM_How_To_Use How To Use
 *  @{

    QPIPSRAM is a framework protocol layer based on QPI psram. It needs to be combined
 with the corresponding driver layer to complete the transmission of the protocol.

 - PSRAM Layer(drv_psram.h only)
 - QPIPSRAM Protocol Layer
 - Controller Host Layer (FSPI, SFC, SPI)
 - QPI Psram

 How to read/write QPI Psram data in SPI mode:
 - consult to psram_test.c

 @} */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "hal_bsp.h"
#include "hal_base.h"

#include "drv_psram.h"
#include "drv_clock.h"

#ifdef RT_USING_QPIPSRAM

/********************* Private MACRO Definition ******************************/
/** @defgroup QPIPSRAM_Private_Macro Private Macro
 *  @{
 */

//#define QPIPSRAM_DEBUG
#ifdef QPIPSRAM_DEBUG
#define QPIPSRAM_DBG(...)     rt_kprintf(__VA_ARGS__)
#else
#define QPIPSRAM_DBG(...)
#endif

/** @} */  // QPIPSRAM_Private_Macro

/********************* Private Structure Definition **************************/
/** @defgroup QPIPSRAM_Private_Structure Private Structure
 *  @{
 */

/** @} */  // QPIPSRAM_Private_Structure

/********************* Private Variable Definition ***************************/
/** @defgroup QPIPSRAM_Private_Macro Variable Macro
 *  @{
 */
struct QPI_PSRAM *s_qpipsram;
struct rt_mutex host_lock;

/** @} */  // QPIPSRAM_Variable_Macro

/********************* Private Function Definition ***************************/
/** @defgroup QPIPSRAM_Private_Function Private Function
 *  @{
 */

rt_size_t qpipsram_read(struct rk_psram_device *dev, rt_off_t pos, rt_uint8_t *data, rt_size_t size)
{
    int ret;

    QPIPSRAM_DBG("%s pos = %08x,size = %08x\n", __func__, pos, size);

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(size != 0);

    rt_mutex_take(&host_lock, RT_WAITING_FOREVER);
    if (s_qpipsram->spi->mode & HAL_SPI_XIP)
    {
        HAL_DCACHE_InvalidateByRange((uint32_t)(pos + s_qpipsram->spi->xipMem), size);
        rt_memcpy(data, (uint32_t *)(pos + s_qpipsram->spi->xipMem), size);
        ret = size;
    }
    else
    {
        ret = HAL_QPIPSRAM_ReadData(s_qpipsram, pos, (void *)data, size);
    }
    rt_mutex_release(&host_lock);
    if (ret != size)
    {
        return -RT_ERROR;
    }
    return size;
}

rt_size_t qpipsram_write(struct rk_psram_device *dev, rt_off_t pos, const rt_uint8_t *data, rt_size_t size)
{
    int ret;

    QPIPSRAM_DBG("%s pos = %08x,size = %08x\n", __func__, pos, size);

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(size != 0);

    rt_mutex_take(&host_lock, RT_WAITING_FOREVER);
    if (s_qpipsram->spi->mode & HAL_SPI_XIP)
    {
        rt_memcpy((uint32_t *)(pos + s_qpipsram->spi->xipMem), data, size);
        HAL_DCACHE_CleanByRange((uint32_t)(pos + s_qpipsram->spi->xipMem), size);
        ret = size;
    }
    else
    {
        ret = HAL_QPIPSRAM_ProgData(s_qpipsram, pos, (void *)data, size);
    }
    rt_mutex_release(&host_lock);
    if (ret != size)
    {
        return -RT_ERROR;
    }
    return size;
}

struct rk_psram_driver_ops qpipsram_ops =
{
    qpipsram_read,
    qpipsram_write,
};

#if defined(RT_USING_QPIPSRAM_SPI_HOST)
static HAL_Status SPI_Xfer(struct QPIPSRAM_HOST *spi, struct HAL_SPI_MEM_OP *op)
{
    struct rt_spi_device *spi_device = (struct rt_spi_device *)spi->userdata;
    struct rt_spi_configuration cfg;
    uint32_t pos = 0;
    const uint8_t *tx_buf = NULL;
    uint8_t *rx_buf = NULL;
    uint8_t op_buf[HAL_SPI_OP_LEN_MAX];
    int32_t op_len;
    int32_t i, ret;

    if (op->data.nbytes)
    {
        if (op->data.dir == HAL_SPI_MEM_DATA_IN)
            rx_buf = op->data.buf.in;
        else
            tx_buf = op->data.buf.out;
    }

    /* rt_kprintf("%s %x %lx\n", __func__, op->cmd.opcode, op->data.nbytes); */
    op_len = sizeof(op->cmd.opcode) + op->addr.nbytes + op->dummy.nbytes;
    if (op_len > HAL_SPI_OP_LEN_MAX)
        return -RT_EINVAL;
    op_buf[pos++] = op->cmd.opcode;

    if (op->addr.nbytes)
    {
        for (i = 0; i < op->addr.nbytes; i++)
            op_buf[pos + i] = op->addr.val >> (8 * (op->addr.nbytes - i - 1));
        pos += op->addr.nbytes;
    }

    if (op->dummy.nbytes)
        memset(&op_buf[pos], 0xff, op->dummy.nbytes);

    cfg.data_width = 8;
    cfg.mode = spi->mode | RT_SPI_MSB;
    cfg.max_hz = spi->speed;
    rt_spi_configure(spi_device, &cfg);

    if (tx_buf)
    {
        ret = rt_spi_send_then_send(spi_device, op_buf, op_len, tx_buf, op->data.nbytes);
        if (ret)
            ret = HAL_ERROR;
    }
    else if (rx_buf)
    {
        ret = rt_spi_send_then_recv(spi_device, op_buf, op_len, rx_buf, op->data.nbytes);
        if (ret)
            ret = HAL_ERROR;
    }
    else
    {
        ret = rt_spi_send(spi_device, op_buf, op_len);
        if (ret != op_len)
            ret = HAL_ERROR;
        else
            ret = HAL_OK;
    }

    /* rt_kprintf("%s finished %d\n", __func__, ret); */

    return ret;
}

static uint32_t qpipsram_adapt(void)
{
    struct rt_spi_device *spi_device = NULL;

#if defined(RT_QPIPSRAM_SPI_DEVICE_NAME)
    spi_device = (struct rt_spi_device *)rt_device_find(RT_QPIPSRAM_SPI_DEVICE_NAME);
#endif
    if (!spi_device)
    {
        rt_kprintf("%s can not find %s\n", __func__, RT_QPIPSRAM_SPI_DEVICE_NAME);

        return RT_EINVAL;
    }

    /* Designated host to QPIPSRAM */
    s_qpipsram->spi->userdata = (void *)spi_device;
    s_qpipsram->spi->mode = HAL_SPI_MODE_3;
    s_qpipsram->spi->xfer = SPI_Xfer;
    if (RT_QPIPSRAM_SPEED > 0 && RT_QPIPSRAM_SPEED <= HAL_SPI_MASTER_MAX_SCLK_OUT)
    {
        nor->spi->speed = RT_QPIPSRAM_SPEED;
    }
    else
    {
        nor->spi->speed = HAL_SPI_MASTER_MAX_SCLK_OUT;
    }

    QPIPSRAM_DBG("%s find %s\n", __func__, RT_QPIPSRAM_SPI_DEVICE_NAME, s_qpipsram->spi->speed);

    /* Init QPI Psram abstract */
    return HAL_QPIPSRAM_Init(s_qpipsram);
}
#elif defined(RT_USING_QPIPSRAM_FSPI_HOST)
static HAL_Status SPI_Xfer(struct QPIPSRAM_HOST *spi, struct HAL_SPI_MEM_OP *op)
{
    struct HAL_FSPI_HOST *host = (struct HAL_FSPI_HOST *)spi->userdata;

    host->mode = spi->mode;
    host->cs = 0;

    return HAL_FSPI_SpiXfer(host, op);
}

static HAL_Status SPI_XipConfig(struct QPIPSRAM_HOST *spi, struct HAL_SPI_MEM_OP *op, uint32_t on)
{
    struct HAL_FSPI_HOST *host = (struct HAL_FSPI_HOST *)spi->userdata;

    host->cs = 0;
    if (op)
    {
        host->xmmcDev[0].type = DEV_PSRAM;
        HAL_FSPI_XmmcSetting(host, op);
    }

    return HAL_FSPI_XmmcRequest(host, on);
}

static HAL_Status qpipsram_adapt(void)
{
    struct HAL_FSPI_HOST *host = NULL;
    uint32_t ret;

    /* Designated host to SPI PSRAM */
#ifdef FSPI1
    host = &g_fspi1Dev;
    host->cs = 0;
#endif

    if (host == NULL)
        return HAL_ERROR;

    if (RT_QPIPSRAM_SPEED > 0 && RT_QPIPSRAM_SPEED <= QPIPSRAM_SPEED_MAX)
    {
        s_qpipsram->spi->speed = RT_QPIPSRAM_SPEED;
    }
    else
    {
        s_qpipsram->spi->speed = QPIPSRAM_SPEED_DEFAULT;
    }
    clk_enable_by_id(host->sclkGate);
    clk_enable_by_id(host->hclkGate);
    clk_enable_by_id(host->xipClkGate);
    clk_set_rate(host->sclkID, s_qpipsram->spi->speed);

    host->xmmcDev[0].type = DEV_PSRAM;
    HAL_FSPI_Init(host);
    s_qpipsram->spi->userdata = (void *)host;
    s_qpipsram->spi->mode = HAL_SPI_MODE_3 | HAL_SPI_TX_QUAD | HAL_SPI_RX_QUAD | HAL_SPI_XIP;
    s_qpipsram->spi->xipMem = host->xipMemData;
    s_qpipsram->spi->xfer = SPI_Xfer;
    s_qpipsram->spi->xipConfig = SPI_XipConfig;

    /* Init SPI psram abstract */
    ret = HAL_QPIPSRAM_Init(s_qpipsram);

    return ret;
}
#else
static uint32_t qpipsram_adapt(void)
{
    return RT_EINVAL;
}
#endif

/** @} */  // QPIPSRAM_Private_Function

/********************* Public Function Definition ****************************/

/** @defgroup QPIPSRAM_Public_Functions Public Functions
 *  @{
 */

/**
 * @brief  Init QPIPSRAM framwork and apply to use.
 * @attention The QPIPSRAM will be enabled when board initialization, do not dynamically switch QPIPSRAM
 *      unless specifically required.
 */
int rt_hw_qpippsram_init(void)
{
    struct rk_psram_device *dev;
    struct QPIPSRAM_HOST *spi;
    int ret;

    s_qpipsram = (struct QPI_PSRAM *)rt_calloc(1, sizeof(*s_qpipsram));
    RT_ASSERT(s_qpipsram);
    spi = (struct QPIPSRAM_HOST *)rt_calloc(1, sizeof(*spi));
    RT_ASSERT(spi);
    s_qpipsram->spi = spi;

    ret = qpipsram_adapt();
    if (ret)
    {
        rt_kprintf("Init adapte error ret= %ld\n", ret);
        goto exit;
    }

    dev = (struct rk_psram_device *)rt_calloc(1, sizeof(*dev));
    RT_ASSERT(dev);

    if (rt_mutex_init(&host_lock, "host_lock", RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        rt_kprintf("Init mutex error\n");
        RT_ASSERT(0);
        rt_free(dev);
        goto exit;
    }

    /* register qpipsram device */
    dev->size = HAL_QPIPSRAM_GetCapacity(s_qpipsram);
    dev->ops = &qpipsram_ops;
    rt_device_register(&dev->parent, "psram", RT_DEVICE_FLAG_STANDALONE);

exit:
    if (ret)
    {
        rt_free(s_qpipsram->spi);
        rt_free(s_qpipsram);
    }

    return ret;
}
INIT_DEVICE_EXPORT(rt_hw_qpippsram_init);

/** @} */  // QPIPSRAM_Public_Function

#endif

/** @} */  // QPIPSRAM

/** @} */  // RKBSP_Common_Driver
