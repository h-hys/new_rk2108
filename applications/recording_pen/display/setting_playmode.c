/*
 * Copyright (c) 2020 Fuzhou Rockchip Electronic Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-24     Jair Wu      First version
 *
 */

#include "setting_playmode.h"

#define DBG_SECTION_NAME    "SETMODE"
#define DBG_LEVEL           DBG_INFO
#include "rtdbg.h"

static lv_obj_t *setting_playmode;
static lv_obj_t *setting_playmode_index;
static int g_pPage = MENU_NONE_PAGE;

struct playmode_label
{
    char text[RECPEN_APP_LANG_MAX][MAX_LABEL_LEN];
    lv_obj_t *label;
    int mode;
};

static struct playmode_label setting_labels[PLAYMODE_MAX] =
{
    {{"关闭循环", "Play once"}, NULL, PLAYMODE_ONCE},
    {{"单曲循环", "Single cycle"}, NULL, PLAYMODE_SINGLECYCLE},
    {{"列表循环", "List cycle"}, NULL, PLAYMODE_LISTCYCLE}
};

static size_t start_label = 0;
static int n_mode = 0;

void setting_playmode_refresh(size_t old, size_t new)
{
    char str[32] = {0};
    int y;
    int i;

    if (old || new)
    {
        LOG_D("Page scrolling");
        lv_obj_del(setting_labels[old].label);
        setting_labels[new].label = lv_label_create(setting_playmode, NULL);
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

    sprintf(str, "%d/%d", n_mode + 1, PLAYMODE_MAX);
    lv_label_set_text(setting_playmode_index, str);

    for (i = start_label; i < MIN(PLAYMODE_MAX, start_label + SETTING_PER_PAGE); i++)
    {
        if (i != n_mode)
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

void setting_playmode_key(size_t type, size_t value)
{
    if (type == TYPE_KEY_PRESS)
    {
        switch (value)
        {
        case KEY_UP:
            if (n_mode <=  0)
                break;
            n_mode--;
            n_mode = setting_labels[n_mode].mode;
            if (n_mode < start_label)
                setting_playmode_refresh(start_label + SETTING_PER_PAGE - 1, n_mode);
            else
                setting_playmode_refresh(0, 0);
            break;
        case KEY_DOWN:
            if (n_mode >= PLAYMODE_MAX - 1)
                break;
            n_mode++;
            n_mode = setting_labels[n_mode].mode;
            if (n_mode > (start_label + SETTING_PER_PAGE - 1))
                setting_playmode_refresh(start_label, n_mode);
            else
                setting_playmode_refresh(0, 0);
            break;
        case KEY_CANCEL:
            if (display_page(g_pPage) == RT_EOK)
                g_pPage = MENU_NONE_PAGE;
            break;
        case KEY_OK:
            if (g_app_info.playmode != n_mode)
            {
                if (n_mode > PLAYMODE_MAX || n_mode < PLAYMODE_ONCE)
                    n_mode = PLAYMODE_ONCE;
                LOG_I("Playback mode %d->%d", g_app_info.playmode, n_mode);
                toast_create(setting_labels[n_mode].text[g_app_info.language], 1500);
                g_app_info.playmode = n_mode;
                save_app_info();
            }
            break;
        default:
            break;
        }
    }
}

void setting_playmode_init(int p_page)
{
    int y;

    n_mode = g_app_info.playmode;
    if (n_mode > PLAYMODE_MAX || n_mode < PLAYMODE_ONCE)
        g_app_info.playmode = n_mode = PLAYMODE_ONCE;
    start_label = 0;
    if (!setting_playmode)
    {
        if (g_pPage == MENU_NONE_PAGE)
            g_pPage = p_page;
        setting_playmode = lv_obj_create(lv_scr_act(), NULL);
        lv_obj_set_size(setting_playmode, SCREEN_W, SCREEN_H);
        lv_obj_set_style(setting_playmode, &page_common_style);
        lv_obj_align(setting_playmode, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        setting_playmode_index = lv_label_create(setting_playmode, NULL);
        lv_label_set_long_mode(setting_playmode_index, LV_LABEL_LONG_BREAK);
        lv_label_set_align(setting_playmode_index, LV_LABEL_ALIGN_CENTER);
        lv_label_set_recolor(setting_playmode_index, true);
        lv_obj_set_size(setting_playmode_index, SETTING_LABEL_W, SETTING_LABEL_H);
        lv_obj_align(setting_playmode_index, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        y = SETTING_LABEL_Y_START;
        for (int i = 0; i < MIN(SETTING_PER_PAGE, PLAYMODE_MAX); i++)
        {
            setting_labels[i].label = lv_label_create(setting_playmode, NULL);
            lv_label_set_long_mode(setting_labels[i].label, LV_LABEL_LONG_BREAK);
            lv_label_set_align(setting_labels[i].label, LV_LABEL_ALIGN_LEFT);
            lv_obj_set_size(setting_labels[i].label, SETTING_LABEL_W, SETTING_LABEL_H);
            lv_obj_align(setting_labels[i].label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, y);
            y += SETTING_LABEL_H + SETTING_LABEL_GAP;
        }
        setting_playmode_refresh(0, 0);
    }
}

void setting_playmode_deinit(void)
{
    if (setting_playmode)
    {
        lv_obj_del(setting_playmode);
        setting_playmode = NULL;
    }
}
