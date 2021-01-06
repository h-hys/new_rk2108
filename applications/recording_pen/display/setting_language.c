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

#include "setting_language.h"

#define DBG_SECTION_NAME    "SETLANG"
#define DBG_LEVEL           DBG_INFO
#include "rtdbg.h"

#define SETTING_NUM             RECPEN_APP_LANG_MAX

static lv_obj_t *setting_language;
static lv_obj_t *setting_language_index;
static int g_pPage = MENU_NONE_PAGE;

static struct recpen_label setting_labels[RECPEN_APP_LANG_MAX] =
{
    {{"中文", "Chinese"}, NULL, NULL},
    {{"英文", "English"}, NULL, NULL},
};

static size_t line = 0;
static size_t start_label = 0;

void setting_language_refresh(size_t old, size_t new)
{
    char str[32] = {0};
    int y;
    int i;

    if (old || new)
    {
        LOG_W("Page scrolling");
        lv_obj_del(setting_labels[old].label);
        setting_labels[new].label = lv_label_create(setting_language, NULL);
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
    lv_label_set_text(setting_language_index, str);

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

void setting_language_key(size_t type, size_t value)
{
    if (type == TYPE_KEY_PRESS)
    {
        switch (value)
        {
        case KEY_UP:
            if (line <=  0)
                break;
            line--;
            if (line < start_label)
                setting_language_refresh(start_label + SETTING_PER_PAGE - 1, line);
            else
                setting_language_refresh(0, 0);
            break;
        case KEY_DOWN:
            if (line >= SETTING_NUM - 1)
                break;
            line++;
            if (line > (start_label + SETTING_PER_PAGE - 1))
                setting_language_refresh(start_label, line);
            else
                setting_language_refresh(0, 0);
            break;
        case KEY_CANCEL:
            if (display_page(g_pPage) == RT_EOK)
                g_pPage = MENU_NONE_PAGE;
            break;
        case KEY_OK:
            if (g_app_info.language != line)
            {
                g_app_info.language = line;
                display_page(MENU_SETTING_LANGUAGE_PAGE);
                save_app_info();
            }
            break;
        default:
            break;
        }
    }
}

void setting_language_init(int p_page)
{
    int y;

    line = g_app_info.language;
    start_label = 0;
    if (!setting_language)
    {
        if (g_pPage == MENU_NONE_PAGE)
            g_pPage = p_page;
        setting_language = lv_obj_create(lv_scr_act(), NULL);

        lv_obj_set_size(setting_language, SCREEN_W, SCREEN_H);
        lv_obj_set_style(setting_language, &page_common_style);
        lv_obj_align(setting_language, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        setting_language_index = lv_label_create(setting_language, NULL);
        lv_label_set_long_mode(setting_language_index, LV_LABEL_LONG_BREAK);
        lv_label_set_align(setting_language_index, LV_LABEL_ALIGN_CENTER);
        lv_label_set_recolor(setting_language_index, true);
        lv_obj_set_size(setting_language_index, SETTING_LABEL_W, SETTING_LABEL_H);
        lv_obj_align(setting_language_index, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        y = SETTING_LABEL_Y_START;
        for (int i = 0; i < MIN(SETTING_PER_PAGE, SETTING_NUM); i++)
        {
            setting_labels[i].label = lv_label_create(setting_language, NULL);
            lv_label_set_long_mode(setting_labels[i].label, LV_LABEL_LONG_BREAK);
            lv_label_set_align(setting_labels[i].label, LV_LABEL_ALIGN_LEFT);
            lv_obj_set_size(setting_labels[i].label, SETTING_LABEL_W, SETTING_LABEL_H);
            lv_obj_align(setting_labels[i].label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, y);
            y += SETTING_LABEL_H + SETTING_LABEL_GAP;
        }
        setting_language_refresh(0, 0);
    }
}

void setting_language_deinit(void)
{
    if (setting_language)
    {
        lv_obj_del(setting_language);
        setting_language = NULL;
    }
}
