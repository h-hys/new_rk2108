/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_snor.c
  * @version V1.1
  * @brief   spi nor interface
  *
  * Change Logs:
  * Date           Author          Notes
  * 2019-02-20     Dingqiang Lin   the first version
  * 2019-06-27     Dingqiang Lin   support FSPI
  * 2019-08-02     Dingqiang Lin   support SPI
  *
  ******************************************************************************
  */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup SNOR
 *  @{
 */

/** @defgroup SNOR_How_To_Use How To Use
 *  @{

    SNOR is a framework protocol layer based on nor flash. It needs to be combined
 with the corresponding driver layer to complete the transmission of the protocol.

 - Littler fs      Elm fat fs
 - MTD Layer       Block Layer
 - SNOR Protocol Layer
 - Controller Host Layer (FSPI, SFC, SPI)
 - Nor flash

 reference to <Rockchip_Developer_Guide_RT-Thread_SPIFLASH_CN.md> to get more informations

 @} */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/mtd_nor.h>

#include "board.h"
#include "hal_bsp.h"
#include "drv_clock.h"
#include "drv_snor.h"
#include "drv_flash_partition.h"
#include "hal_base.h"

#ifdef RT_USING_SNOR

/********************* Private MACRO Definition ******************************/
/** @defgroup SNOR_Private_Macro Private Macro
 *  @{
 */

#ifdef SNOR_DEBUG
#define SNOR_DBG(...)     rt_kprintf(__VA_ARGS__)
#else
#define SNOR_DBG(...)
#endif

/** @} */  // SNOR_Private_Macro

/********************* Private Structure Definition **************************/
/** @defgroup SNOR_Private_Structure Private Structure
 *  @{
 */

static struct rt_mutex sfc_lock;
static struct SPI_NOR *nor;

/** @} */  // SNOR_Private_Structure

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/
/** @defgroup SNOR_Private_Function Private Function
 *  @{
 */

rt_base_t snor_xip_suspend(void)
{
    rt_base_t level = 0;

    if (nor->spi->mode & HAL_SPI_XIP)
    {
        level = rt_hw_interrupt_disable();
        HAL_SNOR_XIPDisable(nor);
    }

    return level;
}

void snor_xip_resume(rt_base_t level)
{
    if (nor->spi->mode & HAL_SPI_XIP)
    {
        HAL_SNOR_XIPEnable(nor);
        rt_hw_interrupt_enable(level);
    }
}

/*
 * The length of buffer is at least 8 bytes.
 */
rt_err_t snor_read_uuid(uint8_t *buf)
{
    int32_t ret;
    rt_base_t level;

    rt_mutex_take(&sfc_lock, RT_WAITING_FOREVER);
    level = snor_xip_suspend();
    ret = HAL_SNOR_ReadUUID(nor, buf);
    snor_xip_resume(level);
    rt_mutex_release(&sfc_lock);

    return ret ? RT_ERROR : RT_EOK;
}

static rt_err_t snor_mtd_read_id(struct rt_mtd_nor_device *dev)
{
    rt_uint8_t id[5];
    rt_base_t level;

    level = snor_xip_suspend();
    HAL_SNOR_ReadID(nor, id);
    snor_xip_resume(level);

    return *(rt_uint32_t *)(id);
}

static rt_size_t snor_mtd_read(struct rt_mtd_nor_device *dev, rt_off_t pos, rt_uint8_t *data, rt_size_t size)
{
    int ret;
    rt_base_t level;

    SNOR_DBG("%s pos = %08x,size = %08x\n", __func__, pos, size);

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(size != 0);

    rt_mutex_take(&sfc_lock, RT_WAITING_FOREVER);
    if (nor->spi->mode & HAL_SPI_XIP)
    {
        HAL_DCACHE_InvalidateByRange((uint32_t)(pos + nor->spi->xipMem), size);
        rt_memcpy(data, (uint32_t *)(pos + nor->spi->xipMem), size);
        ret = size;
    }
    else
    {
        level = snor_xip_suspend();
        ret = HAL_SNOR_ReadData(nor, pos, (void *)data, size);
        snor_xip_resume(level);
    }
    rt_mutex_release(&sfc_lock);
    if (ret != size)
    {
        return -RT_ERROR;
    }
    return size;
}

static rt_size_t snor_mtd_write(struct rt_mtd_nor_device *dev, rt_off_t pos, const rt_uint8_t *data, rt_size_t size)
{
    int ret;
    rt_base_t level;

    SNOR_DBG("%s pos = %08x,size = %08x\n", __func__, pos, size);

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(size != 0);
    /* Buffer should not point to XIP memory */
    if (nor->spi->mode & HAL_SPI_XIP)
    {
        RT_ASSERT((((uint32_t)data + size) < nor->spi->xipMemCode) ||
                  ((uint32_t)data >= (nor->spi->xipMemCode + nor->size)));
        RT_ASSERT((((uint32_t)data + size) < nor->spi->xipMem) ||
                  ((uint32_t)data >= (nor->spi->xipMem + nor->size)));
    }

    rt_mutex_take(&sfc_lock, RT_WAITING_FOREVER);
    level = snor_xip_suspend();
    ret = HAL_SNOR_ProgData(nor, pos, (void *)data, size);
    snor_xip_resume(level);
    rt_mutex_release(&sfc_lock);
    if (ret != size)
    {
        return -RT_ERROR;
    }
    return size;
}

static rt_err_t snor_mtd_erase_sector(struct rt_mtd_nor_device *dev, rt_off_t pos, rt_uint32_t size)
{
    int ret = RT_EOK;
    rt_base_t level;
    uint32_t nsec = size / nor->sectorSize;

    SNOR_DBG("%s pos = %08x,size = %08x %x\n", __func__, pos, size, nor->size);

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(size != 0);

    rt_mutex_take(&sfc_lock, RT_WAITING_FOREVER);
    level = snor_xip_suspend();
    if (pos == 0 && size == nor->size)
    {
        ret = HAL_SNOR_Erase(nor, pos, ERASE_CHIP);
        nsec = 0;
    }
    while (nsec)
    {
        ret = HAL_SNOR_Erase(nor, pos, ERASE_SECTOR);
        if (ret)
        {
            return -RT_ERROR;
        }
        nsec --;
        pos += dev->block_size;
    }
    snor_xip_resume(level);
    rt_mutex_release(&sfc_lock);
    if (ret != RT_EOK)
    {
        return -RT_ERROR;
    }
    return ret;
}

const static struct rt_mtd_nor_driver_ops snor_mtd_ops =
{
    snor_mtd_read_id,
    snor_mtd_read,
    snor_mtd_write,
    snor_mtd_erase_sector,
};

#if defined(HAL_SNOR_FSPI_HOST)

HAL_Status SPI_Xfer(struct SNOR_HOST *spi, struct HAL_SPI_MEM_OP *op)
{
    struct HAL_FSPI_HOST *host = (struct HAL_FSPI_HOST *)spi->userdata;

    host->mode = spi->mode;
    host->cs = 0;
    return HAL_FSPI_SpiXfer(host, op);
}

HAL_Status SPI_XipConfig(struct SNOR_HOST *spi, struct HAL_SPI_MEM_OP *op, uint32_t on)
{
    struct HAL_FSPI_HOST *host = (struct HAL_FSPI_HOST *)spi->userdata;

    host->cs = 0;
    if (op)
        HAL_FSPI_XmmcSetting(host, op);

    return HAL_FSPI_XmmcRequest(host, on);
}

static uint32_t snor_adapt(void)
{
    struct HAL_FSPI_HOST *host = &g_fspi0Dev;
    uint32_t ret;
    rt_base_t level;

    RT_ASSERT(host);

    /* Designated host to SNOR */
    level = rt_hw_interrupt_disable();
    if (RT_SNOR_SPEED > 0 && RT_SNOR_SPEED <= SNOR_SPEED_MAX)
    {
        nor->spi->speed = RT_SNOR_SPEED;
    }
    else
    {
        nor->spi->speed = SNOR_SPEED_DEFAULT;
    }
    clk_set_rate(host->sclkID, nor->spi->speed);

    host->xmmcDev[0].type = DEV_NOR;
    HAL_FSPI_Init(host);
    nor->spi->userdata = (void *)host;
    nor->spi->mode = HAL_SPI_MODE_3;
    nor->spi->xfer = SPI_Xfer;

    if (RT_SNOR_SPEED > SNOR_SPEED_DEFAULT)
    {
        uint8_t idByte[5];
        int32_t right, left;

        HAL_SNOR_Init(nor);
        left = -1;
        for (right = 0; right <= HAL_FSPI_MAX_DELAY_LINE_CELLS; right++)
        {
            HAL_FSPI_SetDelayLines(host, (uint8_t)right);
            HAL_SNOR_ReadID(nor, idByte);
            if (HAL_SNOR_IsFlashSupported(idByte) && left == -1)
            {
                left = right;
            }
            else if (!HAL_SNOR_IsFlashSupported(idByte) && left >= 0)
            {
                break;
            }
        }

        if (left >= 0 && (right - left > 10))
        {
            HAL_FSPI_SetDelayLines(host, (uint8_t)((right + left) / 2));
        }
        else
        {
            HAL_FSPI_DLLDisable(host);
        }
    }
#ifndef RT_SNOR_DUAL_IO
    nor->spi->mode |= (HAL_SPI_TX_QUAD | HAL_SPI_RX_QUAD);
#else
    nor->spi->mode |= HAL_SPI_RX_DUAL;
#endif
#ifdef HAL_FSPI_XIP_ENABLE
    nor->spi->mode |= HAL_SPI_XIP;
    nor->spi->xipConfig = SPI_XipConfig;
    nor->spi->xipMem = host->xipMemData;
    nor->spi->xipMemCode = host->xipMemCode;
#endif

    /* Init spi nor abstract */
    ret = HAL_SNOR_Init(nor);
    if ((ret == HAL_OK) && (nor->spi->mode & HAL_SPI_XIP))
    {
        HAL_SNOR_XIPEnable(nor);
    }
    rt_hw_interrupt_enable(level);

    return ret;
}
#elif defined(HAL_SNOR_SFC_HOST)
static uint32_t snor_adapt()
{
    struct HAL_SFC_HOST *host = (struct HAL_SFC_HOST *)rt_calloc(1, sizeof(*host));

    RT_ASSERT(host);

    /* Designated host to SNOR */
    host->instance = SFC;
    HAL_SFC_Init(host);
    nor->spi->userdata = (void *)host;
    nor->spi->mode = HAL_SPI_MODE_3;
    nor->spi->mode |= (HAL_SPI_TX_QUAD | HAL_SPI_RX_QUAD);
    nor->spi->xfer = HAL_SFC_SpiXfer;

    /* Init spi nor abstract */
    if (HAL_SNOR_Init(nor))
    {
        rt_free(host);
        return RT_ERROR;
    }
    else
    {
        return RT_EOK;
    }
}
#elif defined(HAL_SNOR_SPI_HOST)
HAL_Status SPI_Xfer(struct SNOR_HOST *spi, struct HAL_SPI_MEM_OP *op)
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

//    rt_kprintf("%s %x %lx\n", __func__, op->cmd.opcode, op->data.nbytes);
    op_len = sizeof(op->cmd.opcode) + op->addr.nbytes + op->dummy.nbytes;
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

//    rt_kprintf("%s finished %d\n", __func__, ret);

    return ret;
}

static uint32_t snor_adapt()
{
    struct rt_spi_device *spi_device = NULL;

#if defined(RT_SNOR_SPI_DEVICE_NAME)
    spi_device = (struct rt_spi_device *)rt_device_find(RT_SNOR_SPI_DEVICE_NAME);
#endif
    if (!spi_device)
    {
        rt_kprintf("%s can not find %s\n", __func__, RT_SNOR_SPI_DEVICE_NAME);

        return RT_EINVAL;
    }

    /* Designated host to SNOR */
    nor->spi->userdata = (void *)spi_device;
    nor->spi->mode = HAL_SPI_MODE_3;
    nor->spi->xfer = SPI_Xfer;
    if (RT_SNOR_SPEED > 0 && RT_SNOR_SPEED <= HAL_SPI_MASTER_MAX_SCLK_OUT)
    {
        nor->spi->speed = RT_SNOR_SPEED;
    }
    else
    {
        nor->spi->speed = HAL_SPI_MASTER_MAX_SCLK_OUT;
    }

    /* Init spi nor abstract */
    return HAL_SNOR_Init(nor);
}
#else
static uint32_t snor_adapt()
{
    return RT_EINVAL;
}
#endif

/** @} */  // SNOR_Private_Function

/********************* Public Function Definition ****************************/

/** @defgroup SNOR_Public_Functions Public Functions
 *  @{
 */

/**
 * @brief  Init SNOR framwork and apply to use.
 * @attention The SNOR will be enabled when board initialization, do not dynamically switch SNOR
 *      unless specifically required.
 */
int rt_hw_snor_init(void)
{
    char *dev_name = "snor";
    int ret;
    struct SNOR_HOST *spi;
    struct rt_mtd_nor_device *mtd_dev;

    nor = (struct SPI_NOR *)rt_calloc(1, sizeof(*nor));
    RT_ASSERT(nor);
    spi = (struct SNOR_HOST *)rt_calloc(1, sizeof(*spi));
    RT_ASSERT(spi);
    nor->spi = spi;

    ret = snor_adapt();
    if (ret)
    {
        rt_kprintf("Init adapte error ret= %ld\n", ret);
        goto exit;
    }

    /* flash feature setting */
    if (rt_mutex_init(&(sfc_lock), "snorLock", RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        rt_kprintf("Init mutex error\n");
        RT_ASSERT(0);
    }

    /* dev setting */
    mtd_dev = rt_malloc(sizeof(struct rt_mtd_nor_device));
    RT_ASSERT(mtd_dev);

    mtd_dev->ops = &snor_mtd_ops;
    mtd_dev->block_size   = nor->sectorSize;
    mtd_dev->block_start  = 0;
    mtd_dev->block_end    = HAL_SNOR_GetCapacity(nor) / mtd_dev->block_size;
    rt_mtd_nor_register_device(dev_name, mtd_dev);
    rk_partition_init(mtd_dev);

exit:
    if (ret)
    {
        rt_free(nor->spi);
        rt_free(nor);
    }

    return ret;
}
INIT_DEVICE_EXPORT(rt_hw_snor_init);

/** @} */  // SNOR_Public_Function

#endif

/** @} */  // SNOR

/** @} */  // RKBSP_Common_Driver
