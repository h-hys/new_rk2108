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

#include "setting_rec_seglen.h"

#define DBG_SECTION_NAME    "SETSEG"
#define DBG_LEVEL           DBG_INFO
#include "rtdbg.h"

#define RECORD_SEGLEN_LEVELS    6
#ifdef RECPEN_RECORD_MAX_DURATION
#define RECORD_SEGLEN_OFF       RECPEN_RECORD_MAX_DURATION
#else
#define RECORD_SEGLEN_OFF       0
#endif
#define RECORD_SEGLEN_1         1
#define RECORD_SEGLEN_30        30
#define RECORD_SEGLEN_60        60
#define RECORD_SEGLEN_90        90
#define RECORD_SEGLEN_120       120
#define RECORD_SEGLEN_DEFAULT   RECORD_SEGLEN_OFF

#define SETTING_NUM                 RECORD_SEGLEN_LEVELS

static lv_obj_t *setting_seglen;
static lv_obj_t *setting_seglen_index;
static int g_pPage = MENU_NONE_PAGE;

static struct recpen_label setting_labels[SETTING_NUM] =
{
    {{"关闭循环", "Off"}, NULL, NULL, RECORD_SEGLEN_OFF},
    {{"1分钟", "1 min"}, NULL, NULL, RECORD_SEGLEN_1},
    {{"30分钟", "30 mins"}, NULL, NULL, RECORD_SEGLEN_30},
    {{"60分钟", "60 mins"}, NULL, NULL, RECORD_SEGLEN_60},
    {{"90分钟", "90 mins"}, NULL, NULL, RECORD_SEGLEN_90},
    {{"120分钟", "120 mins"}, NULL, NULL, RECORD_SEGLEN_120},
};

static size_t line = 0;
static size_t start_label = 0;
static int new_seglen = 0;

void reset_seglen(void)
{
    g_app_info.record_seglen = RECORD_SEGLEN_DEFAULT;
}

int get_seglen(void)
{
    int seglen = g_app_info.record_seglen;

    /* Check if seglen is valid */
    for (int i = 0; i < RECORD_SEGLEN_LEVELS; i++)
    {
        if (seglen == setting_labels[i].value)
            return seglen;
    }

    return RECORD_SEGLEN_DEFAULT;
}

void setting_seglen_refresh(size_t old, size_t new)
{
    char str[32] = {0};
    int y;
    int i;

    if (old || new)
    {
        LOG_D("Page scrolling");
        lv_obj_del(setting_labels[old].label);
        setting_labels[new].label = lv_label_create(setting_seglen, NULL);
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
    lv_label_set_text(setting_seglen_index, str);

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

void setting_seglen_key(size_t type, size_t value)
{
    if (type == TYPE_KEY_PRESS)
    {
        switch (value)
        {
        case KEY_UP:
            if (line <=  0)
                break;
            line--;
            new_seglen = setting_labels[line].value;
            if (line < start_label)
                setting_seglen_refresh(start_label + SETTING_PER_PAGE - 1, line);
            else
                setting_seglen_refresh(0, 0);
            break;
        case KEY_DOWN:
            if (line >= SETTING_NUM - 1)
                break;
            line++;
            new_seglen = setting_labels[line].value;
            if (line > (start_label + SETTING_PER_PAGE - 1))
                setting_seglen_refresh(start_label, line);
            else
                setting_seglen_refresh(0, 0);
            break;
        case KEY_CANCEL:
            if (display_page(g_pPage) == RT_EOK)
                g_pPage = MENU_NONE_PAGE;
            break;
        case KEY_OK:
            if (g_app_info.record_seglen != new_seglen)
            {
                g_app_info.record_seglen = new_seglen;
                toast_create(setting_labels[line].text[g_app_info.language], 1500);
                save_app_info();
            }
            break;
        default:
            break;
        }
    }
}

void setting_seglen_init(int p_page)
{
    int y;

    new_seglen = g_app_info.record_seglen;
    line = 0;
    for (int i = 0; i < RECORD_SEGLEN_LEVELS; i++)
    {
        if (new_seglen == setting_labels[i].value)
        {
            line = i;
            break;
        }
    }
    start_label = 0;
    if (!setting_seglen)
    {
        if (g_pPage == MENU_NONE_PAGE)
            g_pPage = p_page;
        setting_seglen = lv_obj_create(lv_scr_act(), NULL);
        lv_obj_set_size(setting_seglen, SCREEN_W, SCREEN_H);
        lv_obj_set_style(setting_seglen, &page_common_style);
        lv_obj_align(setting_seglen, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        setting_seglen_index = lv_label_create(setting_seglen, NULL);
        lv_label_set_long_mode(setting_seglen_index, LV_LABEL_LONG_BREAK);
        lv_label_set_align(setting_seglen_index, LV_LABEL_ALIGN_CENTER);
        lv_label_set_recolor(setting_seglen_index, true);
        lv_obj_set_size(setting_seglen_index, SETTING_LABEL_W, SETTING_LABEL_H);
        lv_obj_align(setting_seglen_index, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        y = SETTING_LABEL_Y_START;
        for (int i = 0; i < MIN(SETTING_PER_PAGE, SETTING_NUM); i++)
        {
            setting_labels[i].label = lv_label_create(setting_seglen, NULL);
            lv_label_set_long_mode(setting_labels[i].label, LV_LABEL_LONG_BREAK);
            lv_label_set_align(setting_labels[i].label, LV_LABEL_ALIGN_LEFT);
            lv_obj_set_size(setting_labels[i].label, SETTING_LABEL_W, SETTING_LABEL_H);
            lv_obj_align(setting_labels[i].label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, y);
            y += SETTING_LABEL_H + SETTING_LABEL_GAP;
        }
        setting_seglen_refresh(0, 0);
    }
}

void setting_seglen_deinit(void)
{
    if (setting_seglen)
    {
        lv_obj_del(setting_seglen);
        setting_seglen = NULL;
    }
}
