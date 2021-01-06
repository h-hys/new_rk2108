/*
 * Copyright (c) 2020 Fuzhou Rockchip Electronic Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-04     Jair Wu      First version
 * 2020-04-24     Jair Wu      Move to system layer
 *
 */
#include "toast.h"

#define DBG_SECTION_NAME    "TOAST"
#define DBG_LEVEL           DBG_INFO
#include "rtdbg.h"

#define TOAST_SIDE_PADDING      LABEL_H
#define TOAST_BUTTOM_PADDING    LABEL_H

struct toast
{
    lv_obj_t *label;
    uint32_t st;
    uint32_t time;
};
static struct toast *g_toast = NULL;
static rt_sem_t g_sem = NULL;
static lv_style_t toast_style;

void toast_create(char *text, uint32_t ms)
{
    if (!g_sem)
        g_sem = rt_sem_create("toast", 1, RT_IPC_FLAG_FIFO);
    rt_sem_take(g_sem, RT_WAITING_FOREVER);
    if (!g_toast)
    {
        g_toast = rt_malloc(sizeof(struct toast));
        if (!g_toast)
        {
            LOG_E("Create toast failed");
            return;
        }
        g_toast->label = lv_label_create(lv_layer_sys(), NULL);
        if (!g_toast->label)
        {
            LOG_E("Create toast label failed");
            rt_free(g_toast);
            g_toast = NULL;
            return;
        }
        lv_style_copy(&toast_style, &label_style_font16);
        toast_style.body.main_color = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF);
        toast_style.body.grad_color = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF);
        toast_style.body.radius = TOAST_SIDE_PADDING / 2;
        toast_style.body.opa = LV_OPA_70;
        toast_style.text.opa = LV_OPA_80;
        toast_style.text.color = LV_COLOR_MAKE(0x00, 0x00, 0x00);
        lv_obj_set_style(g_toast->label, &toast_style);
        lv_label_set_long_mode(g_toast->label, LV_LABEL_LONG_BREAK);
        lv_obj_set_size(g_toast->label, LABEL_W - TOAST_SIDE_PADDING, LABEL_H);
        lv_label_set_align(g_toast->label, LV_LABEL_ALIGN_CENTER);
    }
    g_toast->time = ms;
    g_toast->st = HAL_GetTick();
    lv_label_set_text(g_toast->label, text);
    lv_obj_align(g_toast->label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -TOAST_BUTTOM_PADDING);
    lv_label_set_body_draw(g_toast->label, 1);
    rt_sem_release(g_sem);
}

void toast_refresh(void)
{
    if (!g_sem || !g_toast)
        return;
    rt_sem_take(g_sem, RT_WAITING_FOREVER);
    if ((HAL_GetTick() - g_toast->st) >= g_toast->time)
    {
        lv_obj_del(g_toast->label);
        rt_free(g_toast);
        g_toast = NULL;
    }
    rt_sem_release(g_sem);
}

/* Make sure nobody use toast before destory */
void toast_destory(void)
{
    lv_obj_del(g_toast->label);
    rt_free(g_toast);
    g_toast = NULL;
    rt_sem_delete(g_sem);
    g_sem = NULL;
}
