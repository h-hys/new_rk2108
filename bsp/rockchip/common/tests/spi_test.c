/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    spi_test.c
  * @author  David Wu
  * @version V0.1
  * @date    10-Mar-2019
  * @brief   spi test for pisces
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>

#ifdef RT_USING_COMMON_TEST_SPI

#include <stdbool.h>
#include <drivers/spi.h>
#include "hal_base.h"

struct spi_test_data
{
    struct rt_spi_device *spi_device;
    uint32_t mode;
    uint32_t spi_mode;
    uint32_t bit_first;
    uint32_t max_speed_hz;
};

static struct spi_test_data g_spi_test_data;

static int spi_test_write(const void *txbuf, size_t n)
{
    struct spi_test_data *data = &g_spi_test_data;
    struct rt_spi_device *spi_device = data->spi_device;
    int ret = -RT_ERROR;

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = data->mode | data->spi_mode | data->bit_first;
        cfg.max_hz = data->max_speed_hz;
        rt_spi_configure(spi_device, &cfg);
    }

    /* send data */
    ret = rt_spi_transfer(spi_device, txbuf, RT_NULL, n);

    return (ret == n) ? RT_EOK : ret;
}

static int spi_test_read(void *rxbuf, size_t n)
{
    struct spi_test_data *data = &g_spi_test_data;
    struct rt_spi_device *spi_device = data->spi_device;
    int ret = -RT_ERROR;
    struct rt_spi_configuration cfg;

    cfg.data_width = 8;
    cfg.mode = data->mode | data->spi_mode | data->bit_first;
    cfg.max_hz = data->max_speed_hz;
    rt_spi_configure(spi_device, &cfg);

    /* send data */
    ret = rt_spi_transfer(spi_device, RT_NULL, rxbuf, n);

    return (ret == n) ? RT_EOK : ret;
}

static int spi_test_write_and_read(const void *tx_buf, void *rx_buf, size_t len)
{
    struct spi_test_data *data = &g_spi_test_data;
    struct rt_spi_device *spi_device = data->spi_device;
    int ret = -RT_ERROR;

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = data->mode | data->spi_mode | data->bit_first;
        cfg.max_hz = data->max_speed_hz;
        rt_spi_configure(spi_device, &cfg);
    }

    ret = rt_spi_transfer(spi_device, tx_buf, rx_buf, len);

    return (ret == len) ? RT_EOK : -RT_EIO;
}

static int __unused spi_test_write_then_read(const void *txbuf, size_t tx_n,
        void *rxbuf, size_t rx_n)
{
    struct spi_test_data *data = &g_spi_test_data;
    struct rt_spi_device *spi_device = data->spi_device;
    int ret = -RT_ERROR;

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = data->mode | data->spi_mode | data->bit_first;
        cfg.max_hz = data->max_speed_hz;
        rt_spi_configure(spi_device, &cfg);
    }

    ret = rt_spi_send_then_recv(spi_device, txbuf, tx_n, rxbuf, rx_n);

    return (ret == rx_n) ? RT_EOK : -RT_EIO;
}

static void spi_test_show_usage()
{
    rt_kprintf("1. config spi_device: op_mode, spi_mode, bit_first, speed like:\n");
    /* spi1, cs0, master, mode0, LSB, 1M */
    rt_kprintf("\tspi_test config spi1_0 0 0 0 1000000\n");
    /* spi1, cs1, master, mode3, MSB, 10M */
    rt_kprintf("\tspi_test config spi1_1 0 3 1 10000000\n");
    rt_kprintf("\n");
    rt_kprintf("2. write/read/loop spi_device: times, size like:\n");
    /* send, spi1, cs0, 1times, 255Bytes */
    rt_kprintf("\tspi_test write spi1_0 1 255\n");
    /* send, spi2, cs1, 10times, 512Bytes */
    rt_kprintf("\tspi_test write spi2_1 10 512\n");
    /* read, spi1, cs0, 1times, 255Bytes */
    rt_kprintf("\tspi_test read spi1_0 1 255\n");
    /* loop, spi1, cs0, 1times, 1024Bytes */
    rt_kprintf("\tspi_test loop spi1_0 1 1024\n");
}

void spi_test(int argc, char **argv)
{
    struct spi_test_data *data = &g_spi_test_data;
    struct rt_spi_device *spi_device = RT_NULL;
    char *cmd, *txbuf = NULL, *rxbuf = NULL;
    uint32_t bytes, start_time, end_time, cost_time;
    uint32_t times = 0, size = 0;
    int i;

    if (argc < 5)
        goto out;

    spi_device = (struct rt_spi_device *)rt_device_find(argv[2]);
    if (spi_device ==  RT_NULL)
    {
        rt_kprintf("Did not find device: %s....\n", argv[2]);
        return;
    }
    data->spi_device = spi_device;

    cmd = argv[1];
    if (!rt_strcmp(cmd, "config"))
    {
        int mode, bit_first;

        if (argc < 7)
            goto out;

        mode = atoi(argv[3]);
        if (mode == 0)
            data->mode = RT_SPI_MASTER;
        else
            data->mode = RT_SPI_SLAVE;

        data->spi_mode = atoi(argv[4]);
        bit_first = atoi(argv[5]);
        if (bit_first)
            data->bit_first = RT_SPI_MSB;
        else
            data->bit_first = RT_SPI_LSB;
        data->max_speed_hz = atoi(argv[6]);

        rt_kprintf("spi %s, mode%d, %s, %dHz speed\n",
                   mode ? "slave" : "master", data->spi_mode,
                   bit_first ? "MSB" : "LSB", data->max_speed_hz);
    }
    else if (!rt_strcmp(cmd, "write"))
    {
        times = atoi(argv[3]);
        size = atoi(argv[4]);

        txbuf = (char *)rt_malloc_align(size, 32);
        if (!txbuf)
        {
            rt_kprintf("spi write alloc buf size %d fail\n", size);
            return;
        }

        for (i = 0; i < size; i++)
            txbuf[i] = i % 256;

        start_time = rt_tick_get();
        for (i = 0; i < times; i++)
            spi_test_write(txbuf, size);
        end_time = rt_tick_get();
        cost_time = (end_time - start_time) * 1000 / RT_TICK_PER_SECOND;

        bytes = size * times * 1;
        bytes = bytes / cost_time;
        rt_kprintf("spi write %d*%d cost %ldms speed:%ldKB/S\n", size, times, cost_time, bytes);

        rt_free_align(txbuf);
    }
    else if (!rt_strcmp(cmd, "read"))
    {
        times = atoi(argv[3]);
        size = atoi(argv[4]);

        rxbuf = (char *)rt_malloc_align(size, 32);
        if (!rxbuf)
        {
            rt_kprintf("spi read alloc buf size %d fail\n", size);
            return;
        }

        start_time = rt_tick_get();
        for (i = 0; i < times; i++)
            spi_test_read(rxbuf, size);
        end_time = rt_tick_get();
        cost_time = (end_time - start_time) * 1000 / RT_TICK_PER_SECOND;

        bytes = size * times * 1;
        bytes = bytes / cost_time;
        rt_kprintf("spi read %d*%d cost %ldms speed:%ldKB/S\n", size, times, cost_time, bytes);

        rt_free_align(rxbuf);
    }
    else if (!rt_strcmp(cmd, "loop"))
    {
        times = atoi(argv[3]);
        size = atoi(argv[4]);

        txbuf = (char *)rt_malloc_align(size, 32);
        if (!txbuf)
        {
            rt_kprintf("spi write alloc buf size %d fail\n", size);
            return;
        }

        rxbuf = (char *)rt_malloc_align(size, 32);
        if (!rxbuf)
        {
            rt_kprintf("spi read alloc buf size %d fail\n", size);
            rt_free_align(txbuf);
            return;
        }

        for (i = 0; i < size; i++)
            txbuf[i] = i % 256;

        start_time = rt_tick_get();
        for (i = 0; i < times; i++)
            spi_test_write_and_read(txbuf, rxbuf, size);
        end_time = rt_tick_get();
        cost_time = (end_time - start_time) * 1000 / RT_TICK_PER_SECOND;

        if (rt_memcmp(txbuf, rxbuf, size))
        {
            for (i = 0; i < size; i++)
            {
                if (txbuf[i] !=  rxbuf[i])
                    rt_kprintf("send[%d]: 0x%x, recv[%d]: 0x%x\n", i, txbuf[i], i, rxbuf[i]);
            }
            rt_kprintf("spi loop test fail\n");
        }

        bytes = size * times;
        bytes = bytes / cost_time;
        rt_kprintf("spi loop %d*%d cost %ldms speed:%ldKB/S\n", size, times, cost_time, bytes);

        rt_free_align(txbuf);
        rt_free_align(rxbuf);
    }
    else
    {
        goto out;
    }

    return;
out:
    spi_test_show_usage();
    return;
}

int _at_spi_test(void)
{
    int ret;
    char *buffer;
    size_t size;
    struct spi_test_data *data = &g_spi_test_data;
    struct rt_spi_device *spi_device = RT_NULL;

    spi_device = (struct rt_spi_device *)rt_device_find("spi1_0");
    if (spi_device ==  RT_NULL)
    {
        rt_kprintf("Did not find device: spi1_0....\n");
        return RT_ERROR;
    }
    data->spi_device = spi_device;
    data->mode = RT_SPI_MASTER;
    data->max_speed_hz = 50000000;

    size = 2048;
    buffer = (char *)rt_malloc_align(size, 32);
    if (!buffer)
    {
        rt_kprintf("spi buffer alloc buf size %d fail\n", size);
        return RT_ERROR;
    }

    for (size = 1024; size < 1036; size++)
    {
        ret = spi_test_write(buffer, size);
        if (ret)
        {
            rt_kprintf("spi_test_write %d fail %d\n", size, ret);
            goto out;
        }
        ret = spi_test_read(buffer, size);
        if (ret)
        {
            rt_kprintf("spi_test_read %d fail %d\n", size, ret);
            goto out;
        }
    }
    rt_kprintf("%s test succress\n", __func__);
out:
    rt_free_align(buffer);
    return ret;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(spi_test, spt test cmd);
FINSH_FUNCTION_EXPORT(_at_spi_test, spi test for auto test);
#endif

#endif
