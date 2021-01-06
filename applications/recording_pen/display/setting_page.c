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

#include "setting_page.h"
#include "setting_backlight.h"
#include "setting_rec_seglen.h"

#define DBG_SECTION_NAME    "SETPAGE"
#define DBG_LEVEL           DBG_INFO
#include "rtdbg.h"

enum
{
    SETTING_DELETE,
    SETTING_PLAYMODE,
    SETTING_RECORD,
    SETTING_DATA,
    SETTING_LANGUAGE,
    SETTING_BACKLIGHT,
    SETTING_FORMAT,
    SETTING_RESTORE,
    SETTING_NUM,
};

static lv_obj_t *setting_page;
static lv_obj_t *setting_page_index;
static int g_pPage = MENU_NONE_PAGE;
static size_t line = 0;
static size_t start_label = 0;

static void load_setting_date(void);
static void load_setting_language(void);
static void load_setting_backlight(void);
static void load_setting_playmode(void);
static void load_setting_record(void);
static void format_disk(void);
static void restore_factory(void);
static void unlink_file(void);

static struct recpen_label setting_labels[SETTING_NUM] =
{
    {{"删除文件", "Delete File"}, NULL, unlink_file},
    {{"播放模式", "Playback Mode"}, NULL, load_setting_playmode},
    {{"录音设置", "Record Setting"}, NULL, load_setting_record},
    {{"日期", "Date"}, NULL, load_setting_date},
    {{"语言", "Language"}, NULL, load_setting_language},
    {{"背光设置", "Backlight"}, NULL, load_setting_backlight},
    {{"格式化", "Format"}, NULL, format_disk},
    {{"恢复出厂设置", "Restore Setting"}, NULL, restore_factory},
};
static void load_setting_record(void)
{
    display_page(MENU_SETTING_RECORD_PAGE);
}

static void load_setting_playmode(void)
{
    display_page(MENU_SETTING_PLAYMODE_PAGE);
}

static void load_setting_date(void)
{
    display_page(MENU_SETTING_DATE_PAGE);
}

static void load_setting_language(void)
{
    display_page(MENU_SETTING_LANGUAGE_PAGE);
}

static void load_setting_backlight(void)
{
    display_page(MENU_SETTING_BACKLIGHT_PAGE);
}

static void do_format_disk(int ret)
{
    if (ret == DIALOG_BOX_CONFIRM && rt_device_find("sd0"))
    {
        dfs_mkfs("elm", "sd0");
        mkdir(RECORD_DIR_PATH, 0);
        memset(&g_app_info.file, 0x0, sizeof(struct recpen_file_info));
        sprintf(g_app_info.file.cur_file_name, "No recording file");
        g_app_info.player_total_time = 0;
        g_app_info.player_cur_time = 0;
    }
}

static void format_disk(void)
{
    dialog_box_init(setting_labels[SETTING_FORMAT].text[g_app_info.language],
                    do_format_disk);
}

static void do_restore_factory(int ret)
{
    if (ret == DIALOG_BOX_CONFIRM)
    {
        LOG_I("Restore factory setting");
        g_app_info.language = RECPEN_APP_LANG_CN;
        g_app_info.playmode = PLAYMODE_ONCE;
        g_app_info.brightness = BRIGHTNESS_HW_MAX;
        reset_seglen();
        reset_bl_timeout();
        save_app_info();
        display_page(MENU_SETTING_PAGE);
        /* Reset brightness after page change */
        if (g_app_info.bl_device)
        {
            rt_device_control(g_app_info.bl_device,
                              RTGRAPHIC_CTRL_RECT_UPDATE,
                              &g_app_info.brightness);
        }
    }
}

static void restore_factory(void)
{
    dialog_box_init(setting_labels[SETTING_RESTORE].text[g_app_info.language],
                    do_restore_factory);
}

static void do_unlink_file(int ret)
{
    if (ret == DIALOG_BOX_CONFIRM)
    {
        LOG_I("Remove %s", g_app_info.file.cur_file_name);
        remove_file(RECORD_DIR_PATH, g_app_info.file.cur_file_name);
    }
}

static void unlink_file(void)
{
    if (g_app_info.file.total_file > 0)
    {
        char str[256];
        sprintf(str, "%s \n\"%s\"?%c",
                setting_labels[0].text[g_app_info.language],
                g_app_info.file.cur_file_name,
                '\0');
        dialog_box_init(str, do_unlink_file);
    }
    else
    {
        toast_create("No file\0", 1500);
    }
}

void setting_page_refresh(size_t old, size_t new)
{
    char str[32] = {0};
    int y;
    int i;

    if (old || new)
    {
        LOG_D("Page scrolling");
        lv_obj_del(setting_labels[old].label);
        setting_labels[new].label = lv_label_create(setting_page, NULL);
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
    lv_label_set_text(setting_page_index, str);

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

void setting_page_key(size_t type, size_t value)
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
                setting_page_refresh(start_label + SETTING_PER_PAGE - 1, line);
            else
                setting_page_refresh(0, 0);
            break;
        case KEY_DOWN:
            if (line >= SETTING_NUM - 1)
                break;
            line++;
            if (line > (start_label + SETTING_PER_PAGE - 1))
                setting_page_refresh(start_label, line);
            else
                setting_page_refresh(0, 0);
            break;
        case KEY_CANCEL:
            line = 0;
            start_label = 0;
            if (display_page(g_pPage) == RT_EOK)
                g_pPage = MENU_NONE_PAGE;
            break;
        case KEY_OK:
            if (setting_labels[line].func)
            {
                setting_labels[line].func();
            }
            break;
        default:
            break;
        }
    }

    return;
}

void setting_page_init(int p_page)
{
    int y;

    if (!setting_page)
    {
        if (g_pPage == MENU_NONE_PAGE)
            g_pPage = p_page;
        setting_page = lv_obj_create(lv_scr_act(), NULL);

        lv_obj_set_size(setting_page, SCREEN_W, SCREEN_H);
        lv_obj_set_style(setting_page, &page_common_style);
        lv_obj_align(setting_page, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        setting_page_index = lv_label_create(setting_page, NULL);
        lv_label_set_long_mode(setting_page_index, LV_LABEL_LONG_BREAK);
        lv_label_set_align(setting_page_index, LV_LABEL_ALIGN_CENTER);
        lv_label_set_recolor(setting_page_index, true);
        lv_obj_set_size(setting_page_index, SETTING_LABEL_W, SETTING_LABEL_H);
        lv_obj_align(setting_page_index, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        y = SETTING_LABEL_Y_START;
        for (int i = 0; i < MIN(SETTING_PER_PAGE, SETTING_NUM); i++)
        {
            setting_labels[i].label = lv_label_create(setting_page, NULL);
            lv_label_set_long_mode(setting_labels[i].label, LV_LABEL_LONG_BREAK);
            lv_label_set_align(setting_labels[i].label, LV_LABEL_ALIGN_LEFT);
            lv_obj_set_size(setting_labels[i].label, SETTING_LABEL_W, SETTING_LABEL_H);
            lv_obj_align(setting_labels[i].label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, y);
            y += SETTING_LABEL_H + SETTING_LABEL_GAP;
        }
        setting_page_refresh(0, 0);
    }
}

void setting_page_deinit(void)
{
    if (setting_page)
    {
        lv_obj_del(setting_page);
        setting_page = NULL;
    }
}
