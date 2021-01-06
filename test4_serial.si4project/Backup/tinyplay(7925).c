/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    tinyplay.c
  * @author  sugar zhang
  * @version V0.1
  * @date    6-8-2019
  * @brief   tinyplay for rksoc
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

static uint16_t vol_l = 30, vol_r = 30;
static struct rt_device *s_card = NULL;
#else

#endif

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

struct riff_wave_header
{
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t wave_id;
};

struct chunk_header
{
    uint32_t id;
    uint32_t sz;
};

struct chunk_fmt
{
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

struct playback_config
{
    FILE *file;
    struct rt_device *card;
    uint32_t rate;
    uint32_t channels;
    uint32_t bits;
    uint32_t period_size;
    uint32_t period_count;
    uint32_t total_frames;
};

static player_type_e s_player_state = PLAYER_MAX;

#if 1
static void play_sample(FILE *file, struct rt_device *card, uint32_t channels,
                        uint32_t rate, uint32_t bits, uint32_t period_size,
                        uint32_t period_count, uint32_t total_frames)
{
    struct AUDIO_PARAMS param;
    struct audio_buf abuf;

    rt_err_t ret;
    char *buffer;
    uint32_t size;
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    snd_softvol_t softvol, softvol2;
    snd_pcm_type_t type = SND_PCM_TYPE_SOFTVOL;
#endif

    ret = rt_device_open(card, RT_DEVICE_OFLAG_WRONLY);
    if (ret < 0)
    {
        rt_kprintf("Failed to open %s, err: %d\n", card->parent.name, ret);
        return;
    }

    abuf.period_size = period_size;
    abuf.buf_size = period_size * period_count;
    size = abuf.buf_size * channels * (bits >> 3); /* frames to bytes */
    abuf.buf = rt_malloc_uncache(size);
    if (!abuf.buf)
    {
        rt_kprintf("Buffer alloc failed!\n");
        return;
    }

    param.channels = channels;
    param.sampleRate = rate;
    param.sampleBits = bits;

    ret = rt_device_control(card, RK_AUDIO_CTL_PCM_PREPARE, &abuf);
    RT_ASSERT(ret == RT_EOK);

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_PREPARE, (void *)type);
    RT_ASSERT(ret == RT_EOK);

    softvol.vol_l = vol_l/1;
    softvol.vol_r = vol_r/1;
    rt_kprintf("Set softvol: %d, %d\n", vol_l/1, vol_r/1);
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
        rt_kprintf("Unable to allocate %d bytes\n", size);
        rt_device_close(card);
        return;
    }

    rt_kprintf("Playing sample: %lu ch, %lu hz, %lu bit\n", channels, rate, bits);

    while (1)
    {
    	if(s_player_state == PLAYER_PAUSE)
    	{   		
    		rt_thread_mdelay(50);
    		continue;
    	}
		else if(s_player_state == PLAYER_STOP)
		{
			s_player_state = PLAYER_STOP;
			break;
		}
		
    	if(fread(buffer, 1, size, file) <= 0)
    	{
    		app_led_control(LED_MUTE, LED_CLOSE);
    		break;
    	}		
		
		if (1) //���˵�������������Ȼ�����ǳ�С���������������ס��������
        {
            short *temp = (short *)buffer;
            float data_l;
            float data_r;
            float data_mix;
            for (int i = 0; i < abuf.period_size * channels; i += 2)
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
			
        if (!rt_device_write(card, 0, buffer, abuf.period_size))
        {
            rt_kprintf("Error playing sample\n");
            break;
        }

    }

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_RELEASE, (void *)type);
    RT_ASSERT(ret == RT_EOK);
#endif
    ret = rt_device_control(card, RK_AUDIO_CTL_STOP, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(card, RK_AUDIO_CTL_PCM_RELEASE, NULL);
    RT_ASSERT(ret == RT_EOK);

    rt_free(buffer);
    rt_free_uncache(abuf.buf);
    rt_device_close(card);
	#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
	s_card = NULL;
	#endif
}
#else
static void play_sample(FILE *file, struct rt_device *card, uint32_t channels,
                        uint32_t rate, uint32_t bits, uint32_t period_size,
                        uint32_t period_count, uint32_t total_frames)
{
    struct AUDIO_PARAMS param;
    struct audio_buf abuf;
    rt_err_t ret;
    char *buffer;
    uint32_t size, count;
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    snd_softvol_t softvol, softvol2;
    snd_pcm_type_t type = SND_PCM_TYPE_SOFTVOL;
#endif

    ret = rt_device_open(card, RT_DEVICE_OFLAG_WRONLY);
    if (ret < 0)
    {
        rt_kprintf("Failed to open %s, err: %d\n", card->parent.name, ret);
        return;
    }

    abuf.period_size = period_size;
    abuf.buf_size = period_size * period_count;
    size = abuf.buf_size * channels * (bits >> 3); /* frames to bytes */
    abuf.buf = rt_malloc_uncache(size);
    if (!abuf.buf)
    {
        rt_kprintf("Buffer alloc failed!\n");
        return;
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
        rt_kprintf("Unable to allocate %d bytes\n", size);
        rt_device_close(card);
        return;
    }

    rt_kprintf("Playing sample: %lu ch, %lu hz, %lu bit\n", channels, rate, bits);

    count = total_frames / period_size;
    while (count && (fread(buffer, 1, size, file) > 0))
    {
        if (!rt_device_write(card, 0, buffer, abuf.period_size))
        {
            rt_kprintf("Error playing sample\n");
            break;
        }
        count--;
    }

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
    ret = rt_device_control(card, RK_AUDIO_CTL_PLUGIN_RELEASE, (void *)type);
    RT_ASSERT(ret == RT_EOK);
#endif
    ret = rt_device_control(card, RK_AUDIO_CTL_STOP, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(card, RK_AUDIO_CTL_PCM_RELEASE, NULL);
    RT_ASSERT(ret == RT_EOK);

    rt_free(buffer);
    rt_free_uncache(abuf.buf);
    rt_device_close(card);
}

#endif

static void do_tinyplay(void *arg)
{
    struct playback_config *config = arg;
    FILE *file = config->file;

	s_player_state = PLAYER_START;
    play_sample(file, config->card, config->channels, config->rate,
                config->bits, config->period_size, config->period_count,
                config->total_frames);

	s_player_state = PLAYER_STOP;
    fclose(file);
    rt_free(config);

    rt_kprintf("Done\n");
}

static int tinyplay(int argc, char **argv)
{
    struct playback_config *config;
    struct riff_wave_header riff_wave_header;
    struct chunk_header chunk_header;
    struct chunk_fmt chunk_fmt;
    rt_thread_t tid = RT_NULL;
    uint32_t timeout = UINT32_MAX;
    int more_chunks = 1;
    char card[RT_NAME_MAX] = {0};
    char *filename;
    FILE *file;

    if (argc < 2)
    {
        rt_kprintf("Usage: %s file.wav [-D card] [-p period_size]"
                   " [-n n_periods] [-t seconds]\n", argv[0]);
        return 1;
    }

    filename = argv[1];
    file = fopen(filename, "rb");
    if (!file)
    {
        rt_kprintf("Unable to open file '%s'\n", filename);
        return 1;
    }

    fread(&riff_wave_header, sizeof(riff_wave_header), 1, file);
    if ((riff_wave_header.riff_id != ID_RIFF) ||
            (riff_wave_header.wave_id != ID_WAVE))
    {
        rt_kprintf("Error: '%s' is not a riff/wave file\n", filename);
        fclose(file);
        return 1;
    }

    do
    {
        fread(&chunk_header, sizeof(chunk_header), 1, file);

        switch (chunk_header.id)
        {
        case ID_FMT:
            fread(&chunk_fmt, sizeof(chunk_fmt), 1, file);
            /* If the format header is larger, skip the rest */
            if (chunk_header.sz > sizeof(chunk_fmt))
                fseek(file, chunk_header.sz - sizeof(chunk_fmt), SEEK_CUR);
            break;
        case ID_DATA:
            /* Stop looking for chunks */
            more_chunks = 0;
            break;
        default:
            /* Unknown chunk, skip bytes */
            fseek(file, chunk_header.sz, SEEK_CUR);
        }
    }
    while (more_chunks);

    config = rt_calloc(1, sizeof(*config));
    if (!config)
        return 1;

    config->file = file;
    config->rate = chunk_fmt.sample_rate;
    config->bits = chunk_fmt.bits_per_sample;
    config->channels = chunk_fmt.num_channels;
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
                timeout = atoi(*argv) * chunk_fmt.sample_rate;
        }
        if (strcmp(*argv, "-p") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->period_size = atoi(*argv);
        }
        if (strcmp(*argv, "-n") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                config->period_count = atoi(*argv);
        }
        if (strcmp(*argv, "-D") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                rt_strncpy(card, *argv, RT_NAME_MAX);
        }
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
        if (strcmp(*argv, "-L") == 0)
        {
            argv++;
            argc--;
            if (*argv)
                vol_l = atoi(*argv);
        }
        if (strcmp(*argv, "-R") == 0)
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

    config->total_frames = timeout;

    tid = rt_thread_create("tinyplay", do_tinyplay, config, 4096, RT_THREAD_PRIORITY_MAX / 2, 10);

    if (tid)
        rt_thread_startup(tid);

    return 0;

err:
    if (file)
        fclose(file);
    rt_free(config);
    return 1;
}

static char name[3][30] = {"/mnt/sdcard/test.wav", "/mnt/sdcard/test2.wav", "/mnt/sdcard/test3.wav"};
//static char name[4][30] = {"/test.wav","/mnt/sdcard/123.wav", "/mnt/sdcard/123.wav", "/mnt/sdcard/123.wav"};

static int id = 0;

int tinyplay_audio()
{
	if(s_player_state == PLAYER_START)
	{
		rt_kprintf("playing now");
		s_player_state = PLAYER_STOP;
		return 0;
	}

    struct playback_config *config;
    struct riff_wave_header riff_wave_header;
    struct chunk_header chunk_header;
    struct chunk_fmt chunk_fmt;
    rt_thread_t tid = RT_NULL;
    uint32_t timeout = UINT32_MAX;
    int more_chunks = 1;
    char card[RT_NAME_MAX] = {0};
    FILE *file;

    file = fopen("/mnt/sdcard/test.wav", "rb");
    if (!file)
    {
        rt_kprintf("Unable to open file %s\n", "/mnt/sdcard/test.wav");
        return 1;
    }

    fread(&riff_wave_header, sizeof(riff_wave_header), 1, file);
    if ((riff_wave_header.riff_id != ID_RIFF) ||
            (riff_wave_header.wave_id != ID_WAVE))
    {
        rt_kprintf("Error: /mnt/sdcard/test2.wav is not a riff/wave file\n");
        fclose(file);
        return 1;
    }

    do
    {
        fread(&chunk_header, sizeof(chunk_header), 1, file);

        switch (chunk_header.id)
        {
        case ID_FMT:
            fread(&chunk_fmt, sizeof(chunk_fmt), 1, file);
            /* If the format header is larger, skip the rest */
            if (chunk_header.sz > sizeof(chunk_fmt))
                fseek(file, chunk_header.sz - sizeof(chunk_fmt), SEEK_CUR);
            break;
        case ID_DATA:
            /* Stop looking for chunks */
            more_chunks = 0;
            break;
        default:
            /* Unknown chunk, skip bytes */
            fseek(file, chunk_header.sz, SEEK_CUR);
        }
    }
    while (more_chunks);

    config = rt_calloc(1, sizeof(*config));
    if (!config)
        return 1;

    config->file = file;
    config->rate = chunk_fmt.sample_rate;
    config->bits = chunk_fmt.bits_per_sample;
    config->channels = chunk_fmt.num_channels;
    config->period_size = 1024;
    config->period_count = 4;

    /* parse command line arguments */
    config->card = rt_device_find("audpwmp");
	
    if (!config->card)
    {
        rt_kprintf("Can't find sound device: %s\n", card);
        goto err;
    }
	#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
	s_card = config->card;
	#endif
    config->total_frames = timeout;

    tid = rt_thread_create("tinyplay", do_tinyplay, config, 4096, RT_THREAD_PRIORITY_MAX / 2, 10);

    if (tid)
        rt_thread_startup(tid);

    return 0;

err:
    if (file)
        fclose(file);
    rt_free(config);
    return 1;
}

void player_state_set(player_type_e data)
{
	s_player_state = data;
}

#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
int vol_add2()
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
		s_card = rt_device_find("audpwmp");
		if(s_card == NULL)
		{
			rt_kprintf("s_card NULL\n");
			return 0;
		}
			
	}
	
	rt_device_control(s_card, RK_AUDIO_CTL_PLUGIN_SET_SOFTVOL, &softvol);
						
	rt_kprintf("player_vol_add\n");

	return 0;
}

int vol_sub2()
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
		s_card = rt_device_find("audpwmp");
		if(s_card == NULL)
		{
			rt_kprintf("s_card NULL\n");
			return 0;
		}
			
	}
	
	rt_device_control(s_card, RK_AUDIO_CTL_PLUGIN_SET_SOFTVOL, &softvol);
	
	rt_kprintf("player_vol_sub\n");

	return 0;
}

int vol_set(int argc, char **argv)
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
inline void writel(volatile void *addr, uint32_t val)
{
    *((uint32_t *)addr) = val;
}

inline uint32_t readl(const volatile void *addr)
{
    return *((uint32_t *)addr);
}


void read_register()
{
	rt_kprintf("0x0000 is 0x%x\n", readl(ACDCDIG_BASE + 0x0030));
	rt_kprintf("0x0000 is 0x%x\n", readl(ACDCDIG_BASE + 0x0034));
	rt_kprintf("0x0000 is 0x%x\n", readl(ACDCDIG_BASE + 0x0038));
}

void write_register(int argc, char **argv)
{
	int x = atoi(argv[1]);
	writel(ACDCDIG_BASE + 0x0050, x);
	writel(ACDCDIG_BASE + 0x0054, x);
}

#if 1

#define RT_DEVICE_CTRL_CHAGER_LIMIT_GET     (1) /* Get min, max */
#define RT_DEVICE_CTRL_CHAGER_STATUS_GET    (2) /* Get current configure */
#define RT_DEVICE_CTRL_CHAGER_BATVOL_SET    (3) /* Set battery charge voltage */
#define RT_DEVICE_CTRL_CHAGER_BATCUR_SET    (4) /* Set battery charge current */
#define RT_DEVICE_CTRL_CHAGER_FNSCUR_SET    (5) /* Set battery finish current */
#define RT_DEVICE_CTRL_CHAGER_SRCCUR_SET    (6) /* Set adapter input source current */
struct rt_charger_limit
{
    rt_int32_t bat_vmin; /* battery charge voltage min, max */
    rt_int32_t bat_vmax;
    rt_int32_t bat_cmin; /* battery charge current min, max */
    rt_int32_t bat_cmax;
    rt_int32_t src_cmin; /* adapter input source current min, max */
    rt_int32_t src_cmax;
    rt_int32_t fns_cmin;
    rt_int32_t fns_cmax;
};

struct rt_charger_status
{
    rt_int32_t bat_vol;
    rt_int32_t bat_cur;
    rt_int32_t src_cur;
    rt_int32_t fns_cur;
};
struct rt_fg_status
{
    rt_int8_t  soc; /* state of percent */
    rt_int32_t fcc; /* full charge capacity */
    rt_int32_t current; /* battery current */
    rt_int32_t voltage; /* battery voltage */
    rt_int8_t  present; /* battery existance */
};

#endif

#define RT_DEVICE_CTRL_FG_GET       (1) /* Get fuel gauge status */

int charger_get_info()
{
	struct rt_charger_status status;
	struct rt_charger_limit limit;
	
	rt_device_t device;
	rt_device_t device2;
	struct rt_fg_status st;
	int data = 0;
	device = rt_device_find("charger");
	if (device == RT_NULL)
	{
		return -RT_ERROR;
	}
	device2 = rt_device_find("fuel_gauge");
	if (device2 == RT_NULL)
	{
		return -RT_ERROR;
	}

	rt_device_control(device, RT_DEVICE_CTRL_CHAGER_LIMIT_GET, &limit);
	rt_kprintf("bat cmax %d, cmin %d; vmax %d, vmin %d\n", 
				limit.bat_cmax, limit.bat_cmin,
				limit.bat_vmax, limit.bat_vmin);
	rt_kprintf("src cmax %d, cmin %d; fns vmax %d, vmin %d\n", 
				limit.src_cmax, limit.src_cmin,
				limit.fns_cmax, limit.fns_cmin);
	rt_device_control(device, RT_DEVICE_CTRL_CHAGER_STATUS_GET, &status);
	rt_kprintf("bat_cur %d, bat_vol %d, fns_cur %d, src_cur %d\n", 
		status.bat_cur, status.bat_vol, status.fns_cur, status.src_cur);

	rt_device_control(device2, RT_DEVICE_CTRL_FG_GET, &st);
	rt_kprintf("soc %d, fcc %d, current %d, voltage %d, present %d\n", 
		st.soc, st.fcc, st.current, st.voltage, st.present);

	rt_device_control(device, 7, &data);
	rt_device_close(device);
	rt_device_close(device2);

	return RT_EOK;
}



int charger_set(int argc, char **argv)
{
	rt_kprintf("argc is %d\n", argc);
	if(argc != 3)
	{
		rt_kprintf("usage is charger_set xx xx\n");
		return 0;
	}
		
	int data = atoi(argv[1]);
	int data2 = atoi(argv[2]);
	rt_device_t device;
	device = rt_device_find("charger");
	if (device == RT_NULL)
	{
		return -RT_ERROR;
	}

	printf("data is %d, data2 is %d\n", data, data2);
	if(data >= 3 && data <= 6)
	{
		switch(data)
		{
			case 3:
				rt_device_control(device, RT_DEVICE_CTRL_CHAGER_BATVOL_SET, &data2);
				break;
				
			case 4:
				rt_device_control(device, RT_DEVICE_CTRL_CHAGER_BATCUR_SET, &data2);
				break;
				
			case 5:
				rt_device_control(device, RT_DEVICE_CTRL_CHAGER_FNSCUR_SET, &data2);
				break;
				
			case 6:
				rt_device_control(device, RT_DEVICE_CTRL_CHAGER_SRCCUR_SET, &data2);
				break;
		}
	}
	else
	{
		rt_kprintf("vol range error[3-6], %d\n", data);
		return 0;
	}
	rt_device_close(device);
	return 1;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(tinyplay, play wav file);
MSH_CMD_EXPORT(tinyplay_audio, play wav file);
#ifdef RT_USING_DRIVER_AUDIO_PCM_PLUGIN_SOFTVOL
MSH_CMD_EXPORT(vol_add, add volume);
MSH_CMD_EXPORT(vol_sub, sub volume);
MSH_CMD_EXPORT(vol_set, set volume);
#endif
MSH_CMD_EXPORT(charger_set, charger_set);//charger_set 4 800, charger_set 6 1000/850
MSH_CMD_EXPORT(charger_get_info, charger_get_info);

#endif

#endif
