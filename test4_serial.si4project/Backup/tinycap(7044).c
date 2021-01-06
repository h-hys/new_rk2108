/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    tinycap.c
  * @author  sugar zhang
  * @version V0.1
  * @date    6-8-2019
  * @brief   tinycap for rksoc
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>

#ifdef RT_USING_COMMON_TEST_AUDIO

#include "rk_audio.h"
#include "drv_heap.h"
#include "blsx.h"

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
#include "pcm_plugin_provider.h"

static uint16_t vol_l = 50, vol_r = 50;
#endif

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define FORMAT_PCM 1

static record_type_e record_state = RECORD_MAX;

struct wav_header
{
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t riff_fmt;
    uint32_t fmt_id;
    uint32_t fmt_sz;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t data_id;
    uint32_t data_sz;
};

struct capture_config
{
    FILE *file;
    struct rt_device *card;
    struct rt_device *cardp;
    uint32_t rate;
    uint32_t channels;
    uint32_t bits;
    uint32_t period_size;
    uint32_t period_count;
    uint32_t total_frames;
};

#if 0
static uint32_t capture_sample(FILE *file, struct rt_device *card,
                               uint32_t channels, uint32_t rate,
                               uint32_t bits, uint32_t period_size,
                               uint32_t period_count, uint32_t total_frames)
{
    struct AUDIO_PARAMS param;
    struct audio_buf abuf;
    rt_err_t ret;
    char *buffer;
    uint32_t size;
    uint32_t frames_read = 0, count = 0;
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    snd_softvol_t softvol, softvol2;
    snd_pcm_type_t type = SND_PCM_TYPE_SOFTVOL;
#endif

    ret = rt_device_open(card, RT_DEVICE_OFLAG_RDONLY);
    if (ret < 0)
    {
        rt_kprintf("Failed to open %s, err: %d\n", card->parent.name, ret);
        return 0;
    }

    abuf.period_size = period_size;
    abuf.buf_size = period_size * period_count;
    size = abuf.buf_size * channels * (bits >> 3); /* frames to bytes */
    abuf.buf = rt_malloc_uncache(size);
    if (!abuf.buf)
    {
        rt_kprintf("Buffer alloc failed，size is %d!\n", size);
        return 0;
    }

    param.channels = channels;
    param.sampleRate = rate;
    param.sampleBits = bits;

    ret = rt_device_control(card, RK_AUDIO_CTL_PCM_PREPARE, &abuf);
    RT_ASSERT(ret == RT_EOK);

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_PREPARE, (void *)type);
    RT_ASSERT(ret == RT_EOK);

    softvol.vol_l = vol_l;
    softvol.vol_r = vol_r;
    rt_kprintf("Set softvol: %d, %d\n", vol_l, vol_r);
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_SET_SOFTVOL, &softvol);
    RT_ASSERT(ret == RT_EOK);

    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_GET_SOFTVOL, &softvol2);
    RT_ASSERT(ret == RT_EOK);
    rt_kprintf("Get softvol2: %d, %d\n", softvol2.vol_l, softvol2.vol_r);
#endif

    ret = rt_device_control(card, RK_AUDIO_CTL_HW_PARAMS, &param);
    RT_ASSERT(ret == RT_EOK);

    size = abuf.period_size * channels * (bits >> 3);
    buffer = rt_malloc(size);
    if (!buffer)
    {
        rt_kprintf("Unable to allocate %ld bytes\n", size);
        rt_device_close(card);
        return 0;
    }

    rt_kprintf("Capturing sample: %lu ch, %lu hz, %lu bits\n", channels, rate, bits);

    count = total_frames / period_size;
    while (count && rt_device_read(card, 0, buffer, abuf.period_size))
    {
        if (fwrite(buffer, 1, size, file) != size)
        {
            rt_kprintf("Error capturing sample\n");
            break;
        }
        frames_read += abuf.period_size;
        count--;
    }

    free(buffer);
    rt_free_uncache(abuf.buf);

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_RELEASE, (void *)type);
    RT_ASSERT(ret == RT_EOK);
#endif

    ret = rt_device_control(card, RK_AUDIO_CTL_STOP, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(card, RK_AUDIO_CTL_PCM_RELEASE, NULL);
    RT_ASSERT(ret == RT_EOK);

    rt_device_close(card);

    return frames_read;
}
#else
static uint32_t capture_sample(FILE *file, struct rt_device *card,
                               uint32_t channels, uint32_t rate,
                               uint32_t bits, uint32_t period_size,
                               uint32_t period_count, uint32_t total_frames)
{
    struct AUDIO_PARAMS param;
    struct audio_buf abuf;
    rt_err_t ret;
    char *buffer;
    uint32_t size;
    uint32_t frames_read = 0;
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    snd_softvol_t softvol, softvol2;
    snd_pcm_type_t type = SND_PCM_TYPE_SOFTVOL;
#endif

    ret = rt_device_open(card, RT_DEVICE_OFLAG_RDONLY);
    if (ret < 0)
    {
        rt_kprintf("Failed to open %s, err: %d\n", card->parent.name, ret);
        return 0;
    }

    abuf.period_size = period_size;
    abuf.buf_size = period_size * period_count;
    size = abuf.buf_size * channels * (bits >> 3); /* frames to bytes */
    abuf.buf = rt_malloc_uncache(size);
    if (!abuf.buf)
    {
        rt_kprintf("Buffer alloc failed，size is %d!\n", size);
        return 0;
    }

    param.channels = channels;
    param.sampleRate = rate;
    param.sampleBits = bits;

    ret = rt_device_control(card, RK_AUDIO_CTL_PCM_PREPARE, &abuf);
    RT_ASSERT(ret == RT_EOK);

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_PREPARE, (void *)type);
    RT_ASSERT(ret == RT_EOK);

    softvol.vol_l = vol_l;
    softvol.vol_r = vol_r;
    rt_kprintf("Set softvol: %d, %d\n", vol_l, vol_r);
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_SET_SOFTVOL, &softvol);
    RT_ASSERT(ret == RT_EOK);

    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_GET_SOFTVOL, &softvol2);
    RT_ASSERT(ret == RT_EOK);
    rt_kprintf("Get softvol2: %d, %d\n", softvol2.vol_l, softvol2.vol_r);
#endif

    ret = rt_device_control(card, RK_AUDIO_CTL_HW_PARAMS, &param);
    RT_ASSERT(ret == RT_EOK);

    size = abuf.period_size * channels * (bits >> 3);
    buffer = rt_malloc(size);
    if (!buffer)
    {
        rt_kprintf("Unable to allocate %ld bytes\n", size);
        rt_device_close(card);
        return 0;
    }

    rt_kprintf("Capturing sample: %lu ch, %lu hz, %lu bits\n", channels, rate, bits);

    while (1)
    {		
    
		if(record_state == RECORD_STOP)//终止
		{
			record_state = RECORD_STOP;
			rt_kprintf("capture stop\n");
			break;
		}
		
		rt_device_read(card, 0, buffer, abuf.period_size);

		if(record_state != RECORD_PAUSE)
		{
			if (fwrite(buffer, 1, size, file) != size)
	        {
	            rt_kprintf("Error capturing sample\n");
	            break;
	        }
		}
        		
        frames_read += abuf.period_size;
		
    }

    free(buffer);
    rt_free_uncache(abuf.buf);

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_RELEASE, (void *)type);
    RT_ASSERT(ret == RT_EOK);
#endif

    ret = rt_device_control(card, RK_AUDIO_CTL_STOP, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(card, RK_AUDIO_CTL_PCM_RELEASE, NULL);
    RT_ASSERT(ret == RT_EOK);

    rt_device_close(card);

    return frames_read;
}
#endif

void capture_state_set(record_type_e data)
{
	record_state = data;
}


static void do_tinycap(void *arg)
{
    struct wav_header header;
    struct capture_config *config = arg;
    FILE *file = config->file;
    uint32_t frames;

    header.riff_id = ID_RIFF;
    header.riff_sz = 0;
    header.riff_fmt = ID_WAVE;
    header.fmt_id = ID_FMT;
    header.fmt_sz = 16;
    header.audio_format = FORMAT_PCM;
    header.num_channels = config->channels;
    header.sample_rate = config->rate;

    header.bits_per_sample = config->bits;
    header.byte_rate = (header.bits_per_sample / 8) * config->channels * config->rate;
    header.block_align = config->channels * (header.bits_per_sample / 8);
    header.data_id = ID_DATA;

    /* leave enough room for header */
    fseek(file, sizeof(struct wav_header), SEEK_SET);
	record_state = RECORD_START;
    frames = capture_sample(file, config->card, config->channels,
                            config->rate, config->bits,
                            config->period_size, config->period_count, config->total_frames);

    printf("Captured %ld frames\n", frames);

    /* write header now all information is known */
    header.data_sz = frames * header.block_align;
    header.riff_sz = header.data_sz + sizeof(header) - 8;
	record_state = RECORD_STOP;
    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(struct wav_header), 1, file);

    fclose(file);
    rt_free(config);
}

static int tinycap(int argc, char **argv)
{
    rt_thread_t tid = RT_NULL;
    struct capture_config *config;
    char card[RT_NAME_MAX] = {0};
    uint32_t timeout = 10; /* 10 seconds default */
    FILE *file = RT_NULL;

    if (argc < 2)
    {
        rt_kprintf("Usage: %s file.wav [-D card] [-c channels] "
                   "[-r rate] [-b bits] [-p period_size] [-n n_periods] [-t seconds]\n", argv[0]);
        return 1;
    }

    config = rt_calloc(1, sizeof(*config));
    if (!config)
        return 1;

    file = fopen(argv[1], "wb");
    if (!file)
    {
        rt_kprintf("Unable to create file '%s'\n", argv[1]);
        goto err;
    }

    config->file = file;
    config->rate = 16000;
    config->bits = 16;
    config->channels = 2;
    config->period_size = 1024;
    config->period_count = 4;

    /* parse command line arguments */
    argv += 2;
    argc -= 2;
    while (argc)
    {
        if (strcmp(*argv, "-t") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                timeout = atoi(*argv);
        }
        else if (strcmp(*argv, "-c") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->channels = atoi(*argv);
        }
        else if (strcmp(*argv, "-r") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->rate = atoi(*argv);
        }
        else if (strcmp(*argv, "-b") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->bits = atoi(*argv);
        }
        else if (strcmp(*argv, "-D") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                rt_strncpy(card, *argv, RT_NAME_MAX);
        }
        else if (strcmp(*argv, "-p") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->period_size = atoi(*argv);
        }
        else if (strcmp(*argv, "-n") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->period_count = atoi(*argv);
        }
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
        else if (strcmp(*argv, "-L") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                vol_l = atoi(*argv);
        }
        else if (strcmp(*argv, "-R") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                vol_r = atoi(*argv);
        }
#endif
        argv++;
        argc--;
    }

    config->card = rt_device_find(card);
    if (!config->card)
    {
        rt_kprintf("Can't find sound device: %s\n", card);
        goto err;
    }

    config->total_frames = config->rate * timeout;

    tid = rt_thread_create("tinycap", do_tinycap, config, 4096, RT_THREAD_PRIORITY_MAX / 2, 10);

    if (tid)
        rt_thread_startup(tid);

    return 0;
err:
    if (file)
        fclose(file);
    rt_free(config);
    return 1;
}

static int get_curtime(char *buf)
{
	if(buf == NULL)
		return 0;
	
	time_t timep;   
	struct tm *p; 

	time(&timep);   
	p = localtime(&timep);

	sprintf(buf, "/mnt/sdcard/%d-%d-%d_%d-%d-%d.wav", (1900 + p->tm_year), ( 1 + p->tm_mon), p->tm_mday,
	                         (p->tm_hour), p->tm_min, p->tm_sec); 	
	
	return 0;
}

int tinycap_audio()
{
	if(record_state == RECORD_START)
	{
		rt_kprintf("recording now, start stop\n");
		record_state = RECORD_STOP;
		return 0;	
	}
	static int id = 0;
	char name[100] = {0};
    rt_thread_t tid = RT_NULL;
    struct capture_config *config = NULL;
    uint32_t timeout = 10; /* 10 seconds default */
    FILE *file = RT_NULL;

    config = rt_calloc(1, sizeof(*config));
    if(!config)
        return 1;
	
	//get_curtime(name);

	snprintf(name, sizeof(name), "/mnt/sdcard/record_%d.wav", ++id);
	
    file = fopen(name, "w+");
    if (!file)
    {
        rt_kprintf("Unable to create file %s\n", name);
        goto err;
    }

    config->file = file;
    config->rate = 16000;
    config->bits = 16;
    config->channels = 6;
    config->period_size = 1024;
    config->period_count = 4;

    config->card = rt_device_find("adcc");
    if (!config->card)
    {
        rt_kprintf("Can't find sound device: %s\n", "adcc");
        goto err;
    }

    config->total_frames = config->rate * timeout;

    tid = rt_thread_create("tinycap", do_tinycap, config, 4096, RT_THREAD_PRIORITY_MAX / 2, 10);

    if (tid)
        rt_thread_startup(tid);

    return 0;
err:
    if (file)
        fclose(file);
    rt_free(config);
    return 1;
}

//----------------------------------------------------
struct rt_device *card = NULL;

int test()
{
	struct AUDIO_GAIN_INFO info;
	struct AUDIO_DB_CONFIG db_config;
	int ret ;
	
	if(card == NULL)
	{
		card = rt_device_find("adcc");
		if (!card)
	    {
	        rt_kprintf("Can't find sound device: %s\n", "adcc");
	        return 1;
	    }
	}

	rt_device_open(card, RT_DEVICE_OFLAG_RDONLY);

	ret = rt_device_control(card, RK_AUDIO_CTL_GET_GAIN_INFO, &info);
    if (ret != RT_EOK)
    {
        rt_kprintf("fail to get gain info\n");
        return -RT_ERROR;
    }
	rt_kprintf("maxdB %d, mindB %d, step %d\n", info.maxdB, info.mindB, info.step);

    ret = rt_device_control(card, RK_AUDIO_CTL_GET_GAIN, &db_config);
    if (ret != RT_EOK)
    {
        rt_kprintf("fail to get gain\n");
        return -RT_ERROR;
    }
	rt_kprintf("db %d, ch %d\n", db_config.dB, db_config.ch);
    //db_config.dB = info.mindB + volume * (info.maxdB - info.mindB) / 100;
    //db_config.dB = db_config.dB - db_config.dB % info.step;
    //rt_kprintf("db_config.dB = %d\n", db_config.dB);

	rt_device_close(card);
	return 0;
}

#if 0
#include "drv_pmic.h"

RT_WEAK struct pwr_i2c_desc pmic_pwr_i2c_desc =
{
    .name = "i2c2",//"i2c0",
    .addr = 0x1a,
};

static rt_err_t pmic_i2c_read(struct rt_i2c_bus_device *pmic_i2c_bus,
                              rt_uint8_t addr, rt_uint16_t reg, rt_uint8_t *data)
{
    struct rt_i2c_msg msg[2];
    rt_uint8_t send_buf[2];

    send_buf[0] = (reg & 0xFF);

    msg[0].addr  = addr;
    msg[0].flags = RT_I2C_WR;
    msg[0].len   = 1;
    msg[0].buf   = send_buf;

    msg[1].addr  = addr;
    msg[1].flags = RT_I2C_RD;
    msg[1].len   = 1;
    msg[1].buf   = data;

    if (rt_i2c_transfer(pmic_i2c_bus, msg, 2) == 2)
        return RT_EOK;
    else
        return -RT_ERROR;
}


rt_uint32_t pmic_read(struct rt_i2c_bus_device *pmic_i2c_bus,
                      rt_uint8_t addr, rt_uint16_t reg)
{
    rt_uint8_t data[1] = {0x00};

    if (pmic_i2c_read(pmic_i2c_bus, addr, reg, data) == RT_EOK)
        return data[0];
    else
        return -RT_ERROR;
}

struct pwr_i2c_desc *pmic_get_i2c_desc(void)
{
    struct pwr_i2c_desc *i2c_desc = &pmic_pwr_i2c_desc;

    RT_ASSERT(i2c_desc->name != NULL);

    if (i2c_desc->bus == RT_NULL)
    {
        i2c_desc->bus = rt_i2c_bus_device_find(i2c_desc->name);
        if (i2c_desc->bus == RT_NULL)
        {
            rt_kprintf("PMIC: No %s\n", i2c_desc->name);
            return NULL;
        }
    }

    return i2c_desc;
}

int gb_set()
{
	struct pwr_i2c_desc *i2c;
	rt_int32_t val;
	i2c = pmic_get_i2c_desc();
	if(i2c == NULL)
	{
        rt_kprintf("i2c2 bus is NULL");
        return 1;
    }	
	
	val = pmic_read(i2c->bus, i2c->addr, 0x17);
    if (val < 0)
    {
        rt_kprintf("failed to read reg: 0x%x\n", 0x17);
        return val;
    }
	rt_kprintf("val is %x\n", val);
	return 1;
}
#endif

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL

int cap_vol_set(int argc, char **argv)
{
	rt_kprintf("argc is %d\n", argc);
	if(argc != 2)
	{
		rt_kprintf("usage is vol_set xx\n");
		return 0;
	}
	
	unsigned short data = atoi(argv[1]);

	printf("data is %d\n", data);
	if(data >= 0 && data <= 100)
	{
		vol_l = data;
		vol_r = data;
	}
	else
	{
		rt_kprintf("vol range error[0-100], %d\n", data);
		return 0;
	}
	
	return 1;
}
static struct rt_device *s_card = NULL;

int vol_add_record()
{
	snd_softvol_t softvol = {0};
	
	if(vol_r < 100)
	{
		vol_r += 5;
		vol_l += 5;
		if(vol_r > 100)
			vol_r = 100;
		if(vol_l > 100)
			vol_l = 100;
	}
	softvol.vol_l = vol_r/1;
	softvol.vol_r = vol_r/1;
	
	if(s_card == NULL)
	{
		s_card = rt_device_find("pdmc");
		if(s_card == NULL)
		{
			rt_kprintf("s_card NULL\n");
			return 0;
		}
			
	}
	
	rt_device_control(s_card, RK_AUDIO_CTL_PLUGIN_SET_SOFTVOL, &softvol);
						
	rt_kprintf("record_vol_add\n");

	return 0;
}


int vol_sub_record()
{
	snd_softvol_t softvol = {0};
	if(vol_r > 0)
	{
		vol_r -= 5;
		vol_l -= 5;
		if(vol_r < 0)
			vol_r = 0;
		if(vol_l < 0)
			vol_l = 0;
	}
	softvol.vol_l = vol_l/1;
	softvol.vol_r = vol_r/1;

	if(s_card == NULL)
	{
		s_card = rt_device_find("pdmc");
		if(s_card == NULL)
		{
			rt_kprintf("s_card NULL\n");
			return 0;
		}
			
	}
	
	rt_device_control(s_card, RK_AUDIO_CTL_PLUGIN_SET_SOFTVOL, &softvol);
	
	rt_kprintf("record_vol_sub\n");

	return 0;
}

int vol_set_record(int argc, char **argv)
{
	rt_kprintf("argc is %d\n", argc);
	if(argc != 2)
	{
		rt_kprintf("usage is vol_set xx\n");
		return 0;
	}
	
	snd_softvol_t softvol = {0};
	unsigned short data = atoi(argv[1]);

	printf("data is %d\n", data);
	if(data >= 0 && data <= 100)
	{
		vol_l = data;
		vol_r = data;
	}
	else
	{
		rt_kprintf("vol range error[0-100], %d\n", data);
		return 0;
	}

	softvol.vol_l = vol_l;
	softvol.vol_r = vol_r;

	if(s_card != NULL)
		rt_device_control(s_card, RK_AUDIO_CTL_PLUGIN_SET_SOFTVOL, &softvol);
	
	return 1;
}
#endif
static uint32_t do_capture_play_sample(void *arg)
{
    struct AUDIO_PARAMS param;
    struct audio_buf abuf;
    struct capture_config *config = arg;
    rt_err_t ret;
    char *buffer;
    uint32_t size;
    uint32_t frames_read = 0;
	
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
		snd_softvol_t softvol, softvol2;
		snd_pcm_type_t type = SND_PCM_TYPE_SOFTVOL;
#endif

    ret = rt_device_open(config->card, RT_DEVICE_OFLAG_RDONLY);
    if (ret < 0)
    {
        rt_kprintf("Failed to open %s, err: %d\n", config->card->parent.name, ret);
        return 0;
    }

    ret = rt_device_open(config->cardp, RT_DEVICE_OFLAG_WRONLY);
    if (ret < 0)
    {
        rt_kprintf("Failed to open %s, err: %d\n", config->cardp->parent.name, ret);
        return 0;
    }

    abuf.period_size = config->period_size;
    abuf.buf_size = config->period_size * config->period_count;
    size = abuf.buf_size * config->channels * (config->bits >> 3); /* frames to bytes */
    abuf.buf = rt_malloc_uncache(size);
    if (!abuf.buf)
    {
        rt_kprintf("Buffer alloc failed，size is %d!\n", size);
        return 0;
    }

    param.channels = config->channels;
    param.sampleRate = config->rate;
    param.sampleBits = config->bits;

    ret = rt_device_control(config->card, RK_AUDIO_CTL_PCM_PREPARE, &abuf);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(config->cardp, RK_AUDIO_CTL_PCM_PREPARE, &abuf);
    RT_ASSERT(ret == RT_EOK);

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(config->card, RK_AUDIO_CTL_PLUGIN_PREPARE, (void *)type);
    RT_ASSERT(ret == RT_EOK);

	ret = rt_device_control(config->cardp, RK_AUDIO_CTL_PLUGIN_PREPARE, (void *)type);
    RT_ASSERT(ret == RT_EOK);
#endif

    ret = rt_device_control(config->card, RK_AUDIO_CTL_HW_PARAMS, &param);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(config->cardp, RK_AUDIO_CTL_HW_PARAMS, &param);
    RT_ASSERT(ret == RT_EOK);

    size = abuf.period_size * config->channels * (config->bits >> 3);
    buffer = rt_malloc(size);
    if (!buffer)
    {
        rt_kprintf("Unable to allocate %ld bytes\n", size);
        rt_device_close(config->card);
        rt_device_close(config->cardp);
        return 0;
    }

    rt_kprintf("Capturing sample: %lu ch, %lu hz, %lu bits\n", config->channels, config->rate, config->bits);

    while (1)
    {		
    
		if(record_state == RECORD_STOP)//终止
		{
			record_state = RECORD_STOP;
			rt_kprintf("capture stop\n");
			break;
		}
		
		rt_device_read(config->card, 0, buffer, abuf.period_size);
		
		if (1) //?????????????????????????С????????????????????????
        {
            short *temp = (short *)buffer;
            float data_l;
            float data_r;
            float data_mix;
            for (int i = 0; i < abuf.period_size * 2; i += 2)
            {
                data_l = (float)(temp[i]);
                data_r = (float)(temp[i + 1]);
                if (data_l > 0.0 && data_r > 0.0)
                    data_mix = data_l + data_r - data_l * data_r / INT16_MAX;
                else if (data_l < 0.0 && data_r < 0.0)
                    data_mix = data_l + data_r - data_l * data_r / INT16_MIN;
                else
                    data_mix = data_l + data_r;
                temp[i] = (short)data_mix;
                temp[i + 1] = (short)(-data_mix);
            }
        }
		if(record_state != RECORD_PAUSE)
		{
			if (!rt_device_write(config->cardp, 0, buffer, abuf.period_size))
	        {
	            rt_kprintf("Error playing sample\n");
	            break;
	        }
		}
		
        		
      //  frames_read += abuf.period_size;
		
    }

    free(buffer);
    rt_free_uncache(abuf.buf);

    ret = rt_device_control(config->card, RK_AUDIO_CTL_STOP, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(config->card, RK_AUDIO_CTL_PCM_RELEASE, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(config->cardp, RK_AUDIO_CTL_STOP, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(config->cardp, RK_AUDIO_CTL_PCM_RELEASE, NULL);
    RT_ASSERT(ret == RT_EOK);

    rt_device_close(config->card);
    rt_device_close(config->cardp);

    return frames_read;
}

int capture_play_sample()
{
	if(record_state == RECORD_START)
	{
		rt_kprintf("recording now");
		record_state = RECORD_STOP;
		return 0;	
	}
	
    rt_thread_t tid = RT_NULL;
    struct capture_config *config;
    uint32_t timeout = 10; /* 10 seconds default */

    config = rt_calloc(1, sizeof(*config));
    if (!config)
        return 1;

    config->rate = 16000;
    config->bits = 16;
    config->channels = 4;
    config->period_size = 1024;
    config->period_count = 4;

    config->card = rt_device_find("pdmc");
    if (!config->card)
    {
        rt_kprintf("Can't find sound device: %s\n", "pdmc");
        goto err;
    }
    config->cardp = rt_device_find("pcmp");
    if (!config->cardp)
    {
        rt_kprintf("Can't find sound device: %s\n", "pcmp");
        goto err;
    }

    config->total_frames = config->rate * timeout;

    tid = rt_thread_create("capture_play_sample", do_capture_play_sample, config, 4096, RT_THREAD_PRIORITY_MAX / 2, 10);

    if (tid)
        rt_thread_startup(tid);

    return 0;
err:
    rt_free(config);
    return 1;
}

int play_capture_sample()
{
	if(record_state == RECORD_START)
	{
		rt_kprintf("recording now");
		record_state = RECORD_STOP;
		return 0;	
	}
	
    rt_thread_t tid = RT_NULL;
    struct capture_config *config;
    uint32_t timeout = 10; /* 10 seconds default */

    config = rt_calloc(1, sizeof(*config));
    if (!config)
        return 1;

    config->rate = 16000;
    config->bits = 16;
    config->channels = 2;
    config->period_size = 1024;
    config->period_count = 4;

    config->card = rt_device_find("pcmc");
    if (!config->card)
    {
        rt_kprintf("Can't find sound device: %s\n", "pcmc");
        goto err;
    }
    config->cardp = rt_device_find("audpwmp");
    if (!config->cardp)
    {
        rt_kprintf("Can't find sound device: %s\n", "audpwmp");
        goto err;
    }

    config->total_frames = config->rate * timeout;

    tid = rt_thread_create("play_capture_sample", do_capture_play_sample, config, 4096, RT_THREAD_PRIORITY_MAX / 2, 10);

    if (tid)
        rt_thread_startup(tid);

    return 0;
err:
    rt_free(config);
    return 1;
}
#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(tinycap, capture wav file);
MSH_CMD_EXPORT(tinycap_audio, capture wav file);
MSH_CMD_EXPORT(capture_play_sample, capture play sample);
MSH_CMD_EXPORT(play_capture_sample, play capture sample);
//MSH_CMD_EXPORT(gb_set, gb_set);
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
MSH_CMD_EXPORT(vol_set_record, cap_vol_set);
#endif
#endif

#endif
