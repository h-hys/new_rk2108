#include "littlevgl2rtt.h"
#include "lvgl.h"

static rt_device_t device;
static struct rt_device_graphic_info info;
static struct rt_messagequeue *input_mq;

static lv_disp_buf_t disp_buf;
static lv_color_t buf_1[RT_LV_HOR_RES * 20];

/* Todo: add gpu */
RT_UNUSED static void lcd_fb_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if (x1 > LV_HOR_RES  - 1) return;
    if (y1 > LV_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > LV_HOR_RES  - 1 ? LV_HOR_RES  - 1 : x2;
    int32_t act_y2 = y2 > LV_VER_RES - 1 ? LV_VER_RES - 1 : y2;

    uint32_t x;
    uint32_t y;
    long int location = 0;

    /* 8 bit per pixel */
    if(info.bits_per_pixel == 8)
    {
        uint8_t *fbp8 = (uint8_t*)info.framebuffer;

        for(y = act_y1; y <= act_y2; y++)
        {
            for(x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y) * LV_HOR_RES;
                fbp8[location] = color.full;
            }
        }
    }

    /* 16 bit per pixel */
    else if(info.bits_per_pixel == 16)
    {
        uint16_t *fbp16 = (uint16_t*)info.framebuffer;

        for(y = act_y1; y <= act_y2; y++)
        {
            for(x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y) * LV_HOR_RES;
                fbp16[location] = color.full;
            }
        }
    }

    /* 24 or 32 bit per pixel */
    else if(info.bits_per_pixel == 24 || info.bits_per_pixel == 32)
    {
        uint32_t *fbp32 = (uint32_t*)info.framebuffer;

        for(y = act_y1; y <= act_y2; y++)
        {
            for(x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y) * LV_HOR_RES;
                fbp32[location] = color.full;
            }
        }
    }

#if !(defined(RT_USING_VOP) || defined(RT_USING_SPI_SCREEN))
    struct rt_device_rect_info rect_info;

    rect_info.x = x1;
    rect_info.y = y1;
    rect_info.width = x2 - x1;
    rect_info.height = y2 - y1;
    rt_device_control(device, RTGRAPHIC_CTRL_RECT_UPDATE, &rect_info);
#endif
}

RT_UNUSED static void lcd_fb_map(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*Return if the area is out the screen*/
    if(area->x2 < 0) return;
    if(area->y2 < 0) return;
    if (area->x1 > LV_HOR_RES  - 1) return;
    if (area->y1 > LV_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > LV_HOR_RES  - 1 ? LV_HOR_RES  - 1 : area->x2;
    int32_t act_y2 = area->y2 > LV_VER_RES - 1 ? LV_VER_RES - 1 : area->y2;

    uint32_t x;
    uint32_t y;
    long int location = 0;

    /* 8 bit per pixel */
    if(info.bits_per_pixel == 8)
    {
        uint8_t *fbp8 = (uint8_t*)info.framebuffer;

        for(y = act_y1; y <= act_y2; y++)
        {
            for(x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y) * LV_HOR_RES;
                fbp8[location] = color_p->full;
                color_p++;
            }

            color_p += area->x2 - act_x2;
        }
    }

    /* 16 bit per pixel */
    else if(info.bits_per_pixel == 16)
    {
        uint16_t *fbp16 = (uint16_t*)info.framebuffer;

        for(y = act_y1; y <= act_y2; y++)
        {
            for(x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y) * LV_HOR_RES;
                fbp16[location] = color_p->full;
                color_p++;
            }

            color_p += area->x2 - act_x2;
        }
    }

    /* 24 or 32 bit per pixel */
    else if(info.bits_per_pixel == 24 || info.bits_per_pixel == 32)
    {
        uint32_t *fbp32 = (uint32_t*)info.framebuffer;

        for(y = act_y1; y <= act_y2; y++)
        {
            for(x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y) * LV_HOR_RES;
                fbp32[location] = color_p->full;
                color_p++;
            }

            color_p += area->x2 - act_x2;
        }
    }

#if !(defined(RT_USING_VOP) || defined(RT_USING_SPI_SCREEN))
    struct rt_device_rect_info rect_info;

    rect_info.x = area->x1;
    rect_info.y = area->y1;
    rect_info.width = area->x2 - area->x1;
    rect_info.height = area->y2 - area->y1;
    rt_device_control(device, RTGRAPHIC_CTRL_RECT_UPDATE, &rect_info);
#endif
}

static void lcd_fb_flush(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*Return if the area is out the screen*/
    if(area->x2 < 0) return;
    if(area->y2 < 0) return;
    if (area->x1 > LV_HOR_RES  - 1) return;
    if (area->y1 > LV_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > LV_HOR_RES - 1 ? LV_HOR_RES - 1 : area->x2;
    int32_t act_y2 = area->y2 > LV_VER_RES - 1 ? LV_VER_RES - 1 : area->y2;

    uint32_t x;
    uint32_t y;
    long int location = 0;

    /* 8 bit per pixel */
    if(info.bits_per_pixel == 8)
    {
        uint8_t *fbp8 = (uint8_t*)info.framebuffer;

        for(y = act_y1; y <= act_y2; y++)
        {
            for(x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y) * LV_HOR_RES;
                fbp8[location] = color_p->full;
                color_p++;
            }

            color_p += area->x2 - act_x2;
        }
    }

    /* 16 bit per pixel */
    else if(info.bits_per_pixel == 16)
    {
        uint16_t *fbp16 = (uint16_t*)info.framebuffer;

        for(y = act_y1; y <= act_y2; y++)
        {
            for(x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y) * LV_HOR_RES;
                fbp16[location] = color_p->full;

                color_p++;
            }

            color_p += area->x2 - act_x2;
        }
    }

    /* 24 or 32 bit per pixel */
    else if(info.bits_per_pixel == 24 || info.bits_per_pixel == 32)
    {
        uint32_t *fbp32 = (uint32_t*)info.framebuffer;

        for(y = act_y1; y <= act_y2; y++)
        {
            for(x = act_x1; x <= act_x2; x++)
            {
                location = (x) + (y) * LV_HOR_RES;
                fbp32[location] = color_p->full;
                color_p++;
            }

            color_p += area->x2 - act_x2;
        }
    }

#if !(defined(RT_USING_VOP) || defined(RT_USING_SPI_SCREEN))
    struct rt_device_rect_info rect_info;

    rect_info.x = area->x1;
    rect_info.y = area->y1;
    rect_info.width = area->x2 - area->x1;
    rect_info.height = area->y2 - area->y1;
    rt_device_control(device, RTGRAPHIC_CTRL_RECT_UPDATE, &rect_info);
#endif

    lv_disp_flush_ready(disp_drv);
}

RT_UNUSED static void lcd_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if (x1 > LV_HOR_RES  - 1) return;
    if (y1 > LV_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > LV_HOR_RES  - 1 ? LV_HOR_RES  - 1 : x2;
    int32_t act_y2 = y2 > LV_VER_RES - 1 ? LV_VER_RES - 1 : y2;

    //uint32_t x;
    uint32_t y;

    for(y = act_y1; y <= act_y2; y++)
    {
        rt_graphix_ops(device)->draw_hline((const char *)&color, act_x1, act_x2, y);
    }
}

RT_UNUSED static void lcd_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t * color_p)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if (x1 > LV_HOR_RES  - 1) return;
    if (y1 > LV_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > LV_HOR_RES  - 1 ? LV_HOR_RES  - 1 : x2;
    int32_t act_y2 = y2 > LV_VER_RES - 1 ? LV_VER_RES - 1 : y2;

    //uint32_t x;
    uint32_t y;

    for(y = act_y1; y <= act_y2; y++)
    {
        rt_graphix_ops(device)->blit_line((const char *)&color_p, act_x1, y, act_x2 - act_x1 + 1);
        color_p += (x2 - x1 + 1);
    }
}

static void lcd_flush(struct _disp_drv_t * disp_drv, const lv_area_t *area, lv_color_t * color_p)
{
    /*Return if the area is out the screen*/
    if(area->x2 < 0) return;
    if(area->y2 < 0) return;
    if (area->x1 > LV_HOR_RES  - 1) return;
    if (area->y1 > LV_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > LV_HOR_RES  - 1 ? LV_HOR_RES  - 1 : area->x2;
    int32_t act_y2 = area->y2 > LV_VER_RES - 1 ? LV_VER_RES - 1 : area->y2;

    //uint32_t x;
    uint32_t y;

    for(y = act_y1; y <= act_y2; y++)
    {
        rt_graphix_ops(device)->blit_line((const char *)color_p, act_x1, y, act_x2 - act_x1 + 1);
        color_p += (area->x2 - area->x1 + 1);
    }

    lv_disp_flush_ready(disp_drv);

}

static void lcd_monitor(struct _disp_drv_t * disp_drv, uint32_t time, uint32_t px)
{
}

static void lcd_set_px(struct _disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                      lv_color_t color, lv_opa_t opa)
{
}


static rt_bool_t touch_down = RT_FALSE;
static rt_int16_t last_x = 0;
static rt_int16_t last_y = 0;

static bool input_read(lv_indev_drv_t * indev_driver, lv_indev_data_t *data)
{
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = (touch_down == RT_TRUE) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

    return false; /*Return `false` because we are not buffering and no more data to read*/
}

#if LV_TICK_CUSTOM == 0
static void lvgl_tick_run(void *p)
{
    while(1)
    {
        lv_tick_inc(1);
        rt_thread_delay(1);
    }
}
#endif

void littlevgl2rtt_send_input_event(rt_int16_t x, rt_int16_t y, rt_uint8_t state)
{
    switch(state)
    {
    case LITTLEVGL2RTT_INPUT_UP:
        touch_down = RT_FALSE;
        break;
    case LITTLEVGL2RTT_INPUT_DOWN:
        last_x = x;
        last_y = y;
        touch_down = RT_TRUE;
        break;
    case LITTLEVGL2RTT_INPUT_MOVE:
        last_x = x;
        last_y = y;
        break;
    }
}

#if (LV_LOG_PRINTF != 1)
void littlevgl2rtt_log_register(lv_log_level_t level, const char * file, uint32_t line, const char * dsc)
{
    if(level >= LV_LOG_LEVEL)
    {
      /*Send the logs via serial port*/
      if(level == LV_LOG_LEVEL_ERROR) rt_kprintf("ERROR: ");
      if(level == LV_LOG_LEVEL_WARN)  rt_kprintf("WARNING: ");
      if(level == LV_LOG_LEVEL_INFO)  rt_kprintf("INFO: ");
      if(level == LV_LOG_LEVEL_TRACE) rt_kprintf("TRACE: ");

      rt_kprintf("File:%s #%d: %s\n",file, line, dsc);
      //You can write 'file' and 'line' too similary if required.
    }
}
#endif

rt_err_t littlevgl2rtt_init(const char *name)
{
    rt_err_t ret;

    RT_ASSERT(name != RT_NULL);

    /* LCD Device Init */
    device = rt_device_find(name);
    RT_ASSERT(device != RT_NULL);
    ret = rt_device_open(device, RT_DEVICE_OFLAG_RDWR);
    RT_ASSERT(ret == RT_EOK);

#if defined(RT_USING_VOP)
    ret = lv_set_graphic_info(&info);
    RT_ASSERT(ret == RT_EOK);
#else
    ret = rt_device_control(device, RTGRAPHIC_CTRL_GET_INFO, &info);
    RT_ASSERT(ret == RT_EOK);
#endif

    RT_ASSERT(info.bits_per_pixel ==  8 || info.bits_per_pixel == 16 ||
              info.bits_per_pixel == 24 || info.bits_per_pixel == 32);

    if((info.bits_per_pixel != LV_COLOR_DEPTH) && (info.bits_per_pixel != 32 && LV_COLOR_DEPTH != 24))
    {
        rt_kprintf("Error: framebuffer color depth mismatch! (Should be %d to match with LV_COLOR_DEPTH)",
            info.bits_per_pixel);
        return RT_ERROR;
    }

    /* littlevgl Init */
    lv_init();

#if (LV_LOG_PRINTF != 1)
    /* littlevgl Log Init */
    lv_log_register_print_cb(littlevgl2rtt_log_register);
#endif


    /*A static or global variable to store the buffers*/

    /*Initialize `disp_buf` with the buffer(s) */
    lv_disp_buf_init(&disp_buf, buf_1, NULL, RT_LV_HOR_RES*20);


    /* littlevGL Display device interface */
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    /*Set the resolution of the display*/
    disp_drv.hor_res = RT_LV_HOR_RES;
    disp_drv.ver_res = RT_LV_VER_RES;

    disp_drv.buffer = &disp_buf;            /*Set an initialized buffer*/

    if(info.framebuffer == RT_NULL)
    {
        rt_kprintf("info.frambuffer is NULL\n");
        /* If the display device is a non-framebuffer device,hook the display driver interface according to the specific lcd ic. */
        //disp_drv.disp_fill  = lcd_fill;
        //disp_drv.disp_map   = lcd_map;
        disp_drv.flush_cb = lcd_flush;
        disp_drv.monitor_cb = lcd_monitor;
        disp_drv.set_px_cb = lcd_set_px;
    }
    else
    {
        //disp_drv.disp_fill  = lcd_fb_fill;
        //disp_drv.disp_map   = lcd_fb_map;
        disp_drv.flush_cb = lcd_fb_flush;
        disp_drv.monitor_cb = lcd_monitor;
        //disp_drv.set_px_cb = lcd_set_px;
    }

    lv_disp_drv_register(&disp_drv);

    /* littlevGL Input device interface */
    input_mq = rt_mq_create("lv_input", sizeof(lv_indev_data_t), 256, RT_IPC_FLAG_FIFO);

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);

    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = input_read;

    lv_indev_drv_register(&indev_drv);

#if LV_TICK_CUSTOM == 0
    /* littlevGL Tick thread */
    rt_thread_t thread = RT_NULL;

    thread = rt_thread_create("lv_tick", lvgl_tick_run, RT_NULL, 512, 6, 10);
    if(thread == RT_NULL)
    {
        return RT_ERROR;
    }
    rt_thread_startup(thread);
#endif

    /* Info Print */
    rt_kprintf("[littlevgl2rtt] Welcome to the littlevgl2rtt.\n");
    rt_kprintf("[littlevgl2rtt] You can find latest ver from https://github.com/liu2guang/LittlevGL2RTT.\n");

    return RT_EOK;
}

rt_err_t littlevgl2rtt_deinit(void)
{
    rt_err_t ret = rt_device_close(device);
    RT_ASSERT(ret == RT_EOK);

    return ret;
}

