/*
 * Copyright (c) 2020 Fuzhou Rockchip Electronic Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-04     Jair Wu      First version
 *
 */

#include "setting_backlight_timeout.h"

#define DBG_SECTION_NAME    "SETBLT"
#define DBG_LEVEL           DBG_INFO
#include "rtdbg.h"

#define BACKLIGHT_TIMEOUT_LEVELS    5
#define BACKLIGHT_TIMEOUT_15        15
#define BACKLIGHT_TIMEOUT_30        30
#define BACKLIGHT_TIMEOUT_60        60
#define BACKLIGHT_TIMEOUT_90        90
#define BACKLIGHT_TIMEOUT_120       120
#define BACKLIGHT_TIMEOUT_DEFAULT   BACKLIGHT_TIMEOUT_30

#define SETTING_NUM                 BACKLIGHT_TIMEOUT_LEVELS

static lv_obj_t *setting_backlight;
static lv_obj_t *setting_backlight_index;
static int g_pPage = MENU_NONE_PAGE;

struct bl_level_label
{
    char text[RECPEN_APP_LANG_MAX][MAX_LABEL_LEN];
    lv_obj_t *label;
    int timeout;
};

static struct bl_level_label setting_labels[SETTING_NUM] =
{
    {{"15秒", "15s"}, NULL, BACKLIGHT_TIMEOUT_15},
    {{"30秒", "30s"}, NULL, BACKLIGHT_TIMEOUT_30},
    {{"60秒", "60s"}, NULL, BACKLIGHT_TIMEOUT_60},
    {{"90秒", "90s"}, NULL, BACKLIGHT_TIMEOUT_90},
    {{"120秒", "120s"}, NULL, BACKLIGHT_TIMEOUT_120},
};

void reset_bl_timeout(void)
{
    g_app_info.backlight_timeout = BACKLIGHT_TIMEOUT_DEFAULT;
}

int get_bl_timeout(void)
{
    int timeout = g_app_info.backlight_timeout;

    /* Check if timeout is valid */
    for (int i = 0; i < BACKLIGHT_TIMEOUT_LEVELS; i++)
    {
        if (timeout == setting_labels[i].timeout)
            return timeout;
    }

    return BACKLIGHT_TIMEOUT_DEFAULT;
}

static size_t line = 0;
static size_t start_label = 0;
static int n_timeout = 0;

void setting_bl_timeout_refresh(size_t old, size_t new)
{
    char str[32] = {0};
    int y;
    int i;

    if (old || new)
    {
        LOG_D("Page scrolling");
        lv_obj_del(setting_labels[old].label);
        setting_labels[new].label = lv_label_create(setting_backlight, NULL);
        lv_label_set_long_mode(setting_labels[new].label, LV_LABEL_LONG_BREAK);
        lv_label_set_align(setting_labels[new].label, LV_LABEL_ALIGN_LEFT);
        lv_obj_set_size(setting_labels[new].label, SETTING_LABEL_W, SETTING_LABEL_H);

        if (old > new)
            start_label = new;
        if (old < new)
            start_label = old + 1;
        y = SETTING_LABEL_Y_START;
        for (i = start_label; i < start_label + SETTING_PER_PAGE; i++)
        {
            lv_obj_align(setting_labels[i].label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, y);
            y += SETTING_LABEL_H + SETTING_LABEL_GAP;
        }
    }

    sprintf(str, "%d/%d", line + 1, SETTING_NUM);
    lv_label_set_text(setting_backlight_index, str);

    for (i = start_label; i < MIN(SETTING_NUM, start_label + SETTING_PER_PAGE); i++)
    {
        if (i != line)
        {
            lv_obj_set_style(setting_labels[i].label, &label_style_font20);
            lv_label_set_body_draw(setting_labels[i].label, 0);
        }
        else
        {
            lv_obj_set_style(setting_labels[i].label, &label_style_font20_sel);
            lv_label_set_body_draw(setting_labels[i].label, 1);
        }
        lv_label_set_text(setting_labels[i].label, setting_labels[i].text[g_app_info.language]);
    }
}

void setting_bl_timeout_key(size_t type, size_t value)
{
    if (type == TYPE_KEY_PRESS)
    {
        switch (value)
        {
        case KEY_UP:
            if (line <=  0)
                break;
            line--;
            n_timeout = setting_labels[line].timeout;
            if (line < start_label)
                setting_bl_timeout_refresh(start_label + SETTING_PER_PAGE - 1, line);
            else
                setting_bl_timeout_refresh(0, 0);
            break;
        case KEY_DOWN:
            if (line >= SETTING_NUM - 1)
                break;
            line++;
            n_timeout = setting_labels[line].timeout;
            if (line > (start_label + SETTING_PER_PAGE - 1))
                setting_bl_timeout_refresh(start_label, line);
            else
                setting_bl_timeout_refresh(0, 0);
            break;
        case KEY_CANCEL:
            if (display_page(g_pPage) == RT_EOK)
                g_pPage = MENU_NONE_PAGE;
            break;
        case KEY_OK:
            if (g_app_info.backlight_timeout != n_timeout)
            {
                g_app_info.backlight_timeout = n_timeout;
                uint32_t set_time = n_timeout * 1000;
                if (g_app_info.bl_timer)
                {
                    rt_timer_control(g_app_info.bl_timer, RT_TIMER_CTRL_SET_TIME, &set_time);
                    rt_timer_control(g_app_info.bl_timer, RT_TIMER_CTRL_GET_TIME, &set_time);
                    LOG_I("Set bl timeout %dms", set_time);
                }
                save_app_info();
            }
            break;
        default:
            break;
        }
    }
}

void setting_bl_timeout_init(int p_page)
{
    int y;

    n_timeout = g_app_info.backlight_timeout;
    line = 0;
    for (int i = 0; i < BACKLIGHT_TIMEOUT_LEVELS; i++)
    {
        if (n_timeout == setting_labels[i].timeout)
        {
            line = i;
            break;
        }
    }
    start_label = 0;
    if (!setting_backlight)
    {
        if (g_pPage == MENU_NONE_PAGE)
            g_pPage = p_page;
        setting_backlight = lv_obj_create(lv_scr_act(), NULL);
        lv_obj_set_size(setting_backlight, SCREEN_W, SCREEN_H);
        lv_obj_set_style(setting_backlight, &page_common_style);
        lv_obj_align(setting_backlight, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        setting_backlight_index = lv_label_create(setting_backlight, NULL);
        lv_label_set_long_mode(setting_backlight_index, LV_LABEL_LONG_BREAK);
        lv_label_set_align(setting_backlight_index, LV_LABEL_ALIGN_CENTER);
        lv_label_set_recolor(setting_backlight_index, true);
        lv_obj_set_size(setting_backlight_index, SETTING_LABEL_W, SETTING_LABEL_H);
        lv_obj_align(setting_backlight_index, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        y = SETTING_LABEL_Y_START;
        for (int i = 0; i < MIN(SETTING_PER_PAGE, SETTING_NUM); i++)
        {
            setting_labels[i].label = lv_label_create(setting_backlight, NULL);
            lv_label_set_long_mode(setting_labels[i].label, LV_LABEL_LONG_BREAK);
            lv_label_set_align(setting_labels[i].label, LV_LABEL_ALIGN_LEFT);
            lv_obj_set_size(setting_labels[i].label, SETTING_LABEL_W, SETTING_LABEL_H);
            lv_obj_align(setting_labels[i].label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, y);
            y += SETTING_LABEL_H + SETTING_LABEL_GAP;
        }
        setting_bl_timeout_refresh(0, 0);
    }
}

void setting_bl_timeout_deinit(void)
{
    if (setting_backlight)
    {
        lv_obj_del(setting_backlight);
        setting_backlight = NULL;
    }
}
