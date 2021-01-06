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

#include "main_page.h"

#define DBG_SECTION_NAME    "MAINPAGE"
#define DBG_LEVEL           DBG_INFO
#include "rtdbg.h"

extern image_info_t bat0_img;
extern image_info_t bat25_img;
extern image_info_t bat50_img;
extern image_info_t bat75_img;
extern image_info_t bat100_img;
extern image_info_t play_idle_img;
extern image_info_t play_pause_img;
extern image_info_t play_run_img;
extern image_info_t wifi_img;
extern image_info_t bt_img;
extern image_info_t playonce_img;
extern image_info_t singlecycle_img;
extern image_info_t listcycle_img;

static lv_img_dsc_t bat_img_dsc;
static lv_img_dsc_t play_state_img_dsc;
static lv_img_dsc_t play_mode_img_dsc;
static lv_img_dsc_t wifi_img_dsc;
static lv_img_dsc_t bt_img_dsc;

static lv_obj_t *main_page = NULL;

static lv_obj_t *state_bar = NULL;
static lv_obj_t *divide_line = NULL;

static lv_obj_t *bt_icon = NULL;
static lv_obj_t *wifi_icon = NULL;
static lv_obj_t *bat_icon = NULL;
static lv_obj_t *play_icon = NULL;
static lv_obj_t *playmode_icon = NULL;

static lv_obj_t *file_label = NULL;
static lv_obj_t *file_index_label = NULL;
static lv_obj_t *cur_time_label = NULL;
static lv_obj_t *time_label = NULL;

static lv_obj_t *date_label = NULL;
static lv_obj_t *date_time_label = NULL;

static rt_err_t draw_playmode_icon(void)
{
    size_t value = g_app_info.playmode;
    if (value == PLAYMODE_SINGLECYCLE)
        lvgl_img_dsc_init(&play_mode_img_dsc, &singlecycle_img);
    else if (value == PLAYMODE_LISTCYCLE)
        lvgl_img_dsc_init(&play_mode_img_dsc, &listcycle_img);
    else
        lvgl_img_dsc_init(&play_mode_img_dsc, &playonce_img);

    lv_obj_set_size(playmode_icon, ICON_W, ICON_H);
    lv_obj_align(playmode_icon, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, ICON_Y_START + ICON_Y_GAP + ICON_H);
    lv_img_set_src(playmode_icon, &play_mode_img_dsc);
    lv_obj_invalidate(playmode_icon);

    return RT_EOK;
}

static rt_err_t draw_bat_icon(void)
{
    size_t value = g_app_info.bat;
    if (value >= 95)
        lvgl_img_dsc_init(&bat_img_dsc, &bat100_img);
    else if (value >= 75)
        lvgl_img_dsc_init(&bat_img_dsc, &bat75_img);
    else if (value >= 50)
        lvgl_img_dsc_init(&bat_img_dsc, &bat50_img);
    else if (value >= 25)
        lvgl_img_dsc_init(&bat_img_dsc, &bat25_img);
    else
        lvgl_img_dsc_init(&bat_img_dsc, &bat0_img);

    lv_obj_set_size(bat_icon, ICON_W, ICON_H);
    lv_obj_align(bat_icon, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, ICON_Y_START);
    lv_img_set_src(bat_icon, &bat_img_dsc);
    lv_obj_invalidate(bat_icon);

    return RT_EOK;
}

static rt_err_t draw_bt_icon(void)
{
    lvgl_img_dsc_init(&bt_img_dsc, &bt_img);
    lv_obj_set_size(bt_icon, ICON_W, ICON_H);
    lv_obj_align(bt_icon, NULL, LV_ALIGN_IN_TOP_RIGHT, -(ICON_W + ICON_X_GAP), ICON_Y_START);
    lv_img_set_src(bt_icon, &bt_img_dsc);
    lv_obj_invalidate(bt_icon);

    return RT_EOK;
}

static rt_err_t draw_wifi_icon(void)
{
    lvgl_img_dsc_init(&wifi_img_dsc, &wifi_img);
    lv_obj_set_size(wifi_icon, ICON_W, ICON_H);
    lv_obj_align(wifi_icon, NULL, LV_ALIGN_IN_TOP_RIGHT, -(ICON_W + ICON_X_GAP) * 2, ICON_Y_START);
    lv_img_set_src(wifi_icon, &wifi_img_dsc);
    lv_obj_invalidate(wifi_icon);

    return RT_EOK;
}

static rt_err_t draw_play_icon(void)
{
    player_state_t state = g_app_info.player_state;
    switch (state)
    {
    case PLAYER_STATE_RUNNING:
        lvgl_img_dsc_init(&play_state_img_dsc, &play_run_img);
        break;
    case PLAYER_STATE_PAUSED:
        lvgl_img_dsc_init(&play_state_img_dsc, &play_pause_img);
        break;
    case PLAYER_STATE_IDLE:
    case PLAYER_STATE_ERROR:
    case PLAYER_STATE_STOP:
    default:
        lvgl_img_dsc_init(&play_state_img_dsc, &play_idle_img);
        break;
    }
    lv_obj_set_size(play_icon, ICON_W, ICON_H);
    lv_obj_align(play_icon, NULL, LV_ALIGN_IN_TOP_MID, 0, ICON_Y_START + ICON_Y_GAP + ICON_H);
    lv_img_set_src(play_icon, &play_state_img_dsc);
    lv_obj_invalidate(play_icon);

    return RT_EOK;
}

static rt_err_t draw_state_bar(void)
{
    static lv_point_t line_points[] = {{0, STATE_BAR_H - 1}, {STATE_BAR_W, STATE_BAR_H - 1}};
    static lv_style_t style_line;
    lv_style_copy(&style_line, &lv_style_plain);
    style_line.line.color = LV_COLOR_MAKE(0xff, 0xff, 0xff);
    style_line.line.width = 1;
    style_line.body.main_color = LV_COLOR_BLACK;
    style_line.body.grad_color = LV_COLOR_BLACK;

    lv_obj_set_size(state_bar, STATE_BAR_W, STATE_BAR_H);
    lv_obj_set_style(state_bar, &style_line);
    lv_obj_align(state_bar, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_invalidate(state_bar);
    lv_line_set_points(divide_line, line_points, 2);
    lv_line_set_style(divide_line, 0, &style_line);
    lv_obj_align(divide_line, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_invalidate(divide_line);

    draw_bt_icon();
    draw_wifi_icon();
    draw_bat_icon();
    draw_play_icon();
    draw_playmode_icon();

    return RT_EOK;
}

static rt_err_t draw_file_info(void)
{
    char str[138];
    rt_int16_t y;

    /* draw file name */
    memset((void *)str, 0x0, sizeof(str));
    sprintf(str, "%s", g_app_info.file.cur_file_name);
    y = LABEL_Y_START + LABEL_Y_GAP;

    lv_label_set_long_mode(file_label, LV_LABEL_LONG_SROLL_CIRC);
    lv_label_set_text(file_label, str);
    int16_t width = lv_obj_get_width(file_label);
    if (width < LABEL_W)
    {
        lv_label_set_long_mode(file_label, LV_LABEL_LONG_BREAK);
        lv_obj_set_size(file_label, LABEL_W, LABEL_H);
        y = lv_obj_get_y(file_label);
        lv_obj_align(file_label, NULL, LV_ALIGN_IN_TOP_MID, 0, y);
    }

    /* draw file index */
    memset((void *)str, 0x0, sizeof(str));
    sprintf(str, "%d/%d", g_app_info.file.cur_file, g_app_info.file.total_file);
    lv_label_set_text(file_index_label, str);
    y = lv_obj_get_y(file_index_label);
    lv_obj_align(file_index_label, NULL, LV_ALIGN_IN_TOP_MID, 0, y);

    return RT_EOK;
}

static void draw_audio_time(void)
{
    char str[32];
    rt_int16_t y;
    uint32_t time;

    /* draw current time */
    memset((void *)str, 0x0, sizeof(str));
    time = g_app_info.player_cur_time / 1000;
    uint16_t hour = time / 60 / 60;
    time %= 60 * 60;
    uint16_t min = time / 60;
    uint16_t sec = time % 60;
    sprintf(str, "%02d:%02d:%02d", hour, min, sec);
    lv_label_set_text(cur_time_label, str);
    y = lv_obj_get_y(cur_time_label);
    lv_obj_align(cur_time_label, NULL, LV_ALIGN_IN_TOP_MID, 0, y);

    /* draw file duration */
    memset((void *)str, 0x0, sizeof(str));
    time = g_app_info.player_total_time / 1000;
    hour = time / 60 / 60;
    time %= 60 * 60;
    min = time / 60;
    sec = time % 60;
    sprintf(str, "%02d:%02d:%02d", hour, min, sec);
    lv_label_set_text(time_label, str);
    y = lv_obj_get_y(time_label);
    lv_obj_align(time_label, NULL, LV_ALIGN_IN_TOP_MID, 0, y);
}

static void draw_date(void)
{
    char str[32];
    rt_int16_t y;

    memset((void *)str, 0x0, sizeof(str));
    sprintf(str, "%02d-%02d-%02d",
            g_app_info.date->tm_year + 1900,
            g_app_info.date->tm_mon + 1,
            g_app_info.date->tm_mday);
    lv_label_set_text(date_label, str);
    y = lv_obj_get_y(date_label);
    lv_obj_align(date_label, NULL, LV_ALIGN_IN_TOP_MID, 0, y);

    memset((void *)str, 0x0, sizeof(str));
    sprintf(str, "%02d:%02d:%02d",
            g_app_info.date->tm_hour,
            g_app_info.date->tm_min,
            g_app_info.date->tm_sec);
    lv_label_set_text(date_time_label, str);
    y = lv_obj_get_y(date_time_label);
    lv_obj_align(date_time_label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, y);
}

void main_page_key(size_t type, size_t value)
{
    if (type == TYPE_KEY_PRESS)
    {
        switch (value)
        {
        case KEY_RECORD:
            if (rootfs_check())
                goto NO_ROOTFS;
            recpen_player_stop();
            display_page(MENU_REC_PAGE);
            break;
        case KEY_UP:
            if (rootfs_check())
                goto NO_ROOTFS;
            recpen_player_stop();
            if (recpen_next_file() == RT_EOK)
            {
                g_app_info.player_total_time = recpen_player_get_total_time(g_app_info.file.cur_file_name);
                g_app_info.player_cur_time = 0;
                main_page_refresh(TYPE_REFRESH, REFRESH_PLAY_STATE | REFRESH_MEDIA_INFO | REFRESH_FILE_INFO);
            }
            else
            {
                toast_create("No file\0", 1500);
            }
            break;
        case KEY_DOWN:
            if (rootfs_check())
                goto NO_ROOTFS;
            recpen_player_stop();
            if (recpen_prev_file() == RT_EOK)
            {
                g_app_info.player_total_time = recpen_player_get_total_time(g_app_info.file.cur_file_name);
                g_app_info.player_cur_time = 0;
                main_page_refresh(TYPE_REFRESH, REFRESH_PLAY_STATE | REFRESH_MEDIA_INFO | REFRESH_FILE_INFO);
            }
            else
            {
                toast_create("No file\0", 1500);
            }
            break;
        case KEY_MENU:
            recpen_player_stop();
            display_page(MENU_SETTING_PAGE);
            break;
        case KEY_PLAY:
            if (rootfs_check())
                goto NO_ROOTFS;
            switch (g_app_info.player_state)
            {
            case PLAYER_STATE_RUNNING:
                LOG_I("Player pause");
                g_app_info.player_state = PLAYER_STATE_PAUSED;
                player_pause(g_app_info.player);
                break;
            case PLAYER_STATE_PAUSED:
                LOG_I("Player resume");
                g_app_info.player_state = PLAYER_STATE_RUNNING;
                player_resume(g_app_info.player);
                break;
            case PLAYER_STATE_IDLE:
            case PLAYER_STATE_ERROR:
            case PLAYER_STATE_STOP:
            default:
                LOG_I("Player start");
                g_app_info.player_state = PLAYER_STATE_RUNNING;
                recpen_player_prepare(g_app_info.file.cur_file_name, 0);
                break;
            }
            main_page_refresh(TYPE_REFRESH, REFRESH_PLAY_STATE);
            break;
        default:
            break;
        }
    }

    return;
NO_ROOTFS:
    toast_create("No rootfs, please check", 3000);
}

void main_page_refresh(size_t type, size_t value)
{
    switch (type)
    {
    case TYPE_REFRESH:
        if (value & REFRESH_DATE)
            draw_date();
        if (value & REFRESH_WIFI)
            draw_wifi_icon();
        if (value & REFRESH_BT)
            draw_bt_icon();
        if (value & REFRESH_BAT)
            draw_bat_icon();
        if (value & REFRESH_PLAY_STATE)
            draw_play_icon();
        if (value & REFRESH_MEDIA_INFO)
            draw_audio_time();
        if (value & REFRESH_FILE_INFO)
            draw_file_info();
        lv_obj_invalidate(state_bar);
        break;
    case TYPE_PLAYBACK_DONE:
        LOG_I("Playback callback");
        switch (g_app_info.playmode)
        {
        case PLAYMODE_SINGLECYCLE:
            LOG_I("Signle cycle");
            g_app_info.player_state = PLAYER_STATE_RUNNING;
            recpen_player_prepare(g_app_info.file.cur_file_name, 0);
            main_page_refresh(TYPE_REFRESH, REFRESH_PLAY_STATE);
            break;
        case PLAYMODE_LISTCYCLE:
            LOG_I("List cycle");
            if (recpen_next_file() == RT_EOK)
            {
                g_app_info.player_cur_time = 0;
                g_app_info.player_total_time = recpen_player_get_total_time(g_app_info.file.cur_file_name);
                g_app_info.player_state = PLAYER_STATE_RUNNING;
                main_page_refresh(TYPE_REFRESH, REFRESH_PLAY_STATE | REFRESH_MEDIA_INFO | REFRESH_FILE_INFO);
                rt_thread_mdelay(200);
                recpen_player_prepare(g_app_info.file.cur_file_name, 0);
            }
            break;
        case PLAYMODE_ONCE:
        default:
            LOG_I("PLay once");
            g_app_info.player_state = PLAYER_STATE_IDLE;
            main_page_refresh(TYPE_REFRESH, REFRESH_PLAY_STATE);
            break;
        }
        break;
    default:
        break;
    }
}

void main_page_init(int p_page)
{
    rt_int16_t y;

    if (!main_page)
    {
        main_page = lv_obj_create(lv_scr_act(), NULL);

        lv_obj_set_size(main_page, SCREEN_W, SCREEN_H);
        lv_obj_set_style(main_page, &page_common_style);
        lv_obj_align(main_page, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
        lv_obj_invalidate(main_page);

        state_bar = lv_obj_create(main_page, NULL);
        lv_obj_set_style(state_bar, &page_common_style);

        divide_line = lv_line_create(state_bar, NULL);
        bt_icon = lv_img_create(state_bar, NULL);
        wifi_icon = lv_img_create(state_bar, NULL);
        bat_icon = lv_img_create(state_bar, NULL);
        play_icon = lv_img_create(state_bar, NULL);
        playmode_icon = lv_img_create(state_bar, NULL);

        file_label = lv_label_create(main_page, NULL);
        y = LABEL_Y_START + LABEL_Y_GAP;
        lv_label_set_long_mode(file_label, LV_LABEL_LONG_SROLL_CIRC);
        lv_label_set_align(file_label, LV_LABEL_ALIGN_CENTER);
        lv_obj_set_size(file_label, LABEL_W, LABEL_H);
        lv_obj_align(file_label, NULL, LV_ALIGN_IN_TOP_MID, 0, y);

        file_index_label = lv_label_create(main_page, NULL);
        y += LABEL_Y_GAP + LABEL_H;
        lv_label_set_long_mode(file_index_label, LV_LABEL_LONG_EXPAND);
        lv_obj_set_size(file_index_label, LABEL_W, LABEL_H);
        lv_obj_align(file_index_label, NULL, LV_ALIGN_IN_TOP_MID, 0, y);

        cur_time_label = lv_label_create(main_page, NULL);
        y += LABEL_Y_GAP + LABEL_H;
        lv_label_set_long_mode(cur_time_label, LV_LABEL_LONG_EXPAND);
        lv_obj_set_size(cur_time_label, LABEL_W, LABEL_H);
        lv_obj_align(cur_time_label, NULL, LV_ALIGN_IN_TOP_MID, 0, y);
        lv_obj_set_style(cur_time_label, &label_style_font20_hl);

        time_label = lv_label_create(main_page, NULL);
        y += LABEL_Y_GAP + LABEL_H;
        lv_label_set_long_mode(time_label, LV_LABEL_LONG_EXPAND);
        lv_obj_set_size(time_label, LABEL_W, LABEL_H);
        lv_obj_align(time_label, NULL, LV_ALIGN_IN_TOP_MID, 0, y);
        lv_obj_set_style(time_label, &label_style_font20_hl);

        date_label = lv_label_create(main_page, NULL);
        lv_label_set_long_mode(date_label, LV_LABEL_LONG_EXPAND);
        lv_obj_align(date_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
        lv_obj_set_style(date_label, &label_style_font16);

        date_time_label = lv_label_create(main_page, NULL);
        lv_label_set_long_mode(date_time_label, LV_LABEL_LONG_EXPAND);
        lv_obj_set_style(date_time_label, &label_style_font16);
    }
    draw_state_bar();
    draw_file_info();
    draw_audio_time();
    draw_date();
}

void main_page_deinit(void)
{
    if (main_page)
    {
        /* this function will delete children too */
        lv_obj_del(main_page);
        main_page = NULL;
    }
}
