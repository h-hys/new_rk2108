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

#include "setting_date.h"

#define DBG_SECTION_NAME    "SETLANG"
#define DBG_LEVEL           DBG_INFO
#include "rtdbg.h"

static lv_obj_t *setting_date;
static lv_obj_t *setting_date_label[5];
static int g_pPage = MENU_NONE_PAGE;
static struct tm date;

static size_t g_select = 0;

void setting_date_refresh(void)
{
    char str[5][32];
    int i;

    sprintf(str[0], "%04d  Y%c", date.tm_year, '\0');
    sprintf(str[1], "%02d  M%c", date.tm_mon + 1, '\0');
    sprintf(str[2], "%02d  D%c", date.tm_mday, '\0');
    sprintf(str[3], "%02d  h%c", date.tm_hour, '\0');
    sprintf(str[4], "%02d  m%c", date.tm_min, '\0');

    for (i = 0; i < 5; i++)
    {
        if (i != g_select)
        {
            lv_obj_set_style(setting_date_label[i], &label_style_font20);
            lv_label_set_body_draw(setting_date_label[i], 0);
        }
        else
        {
            lv_obj_set_style(setting_date_label[i], &label_style_font20_sel);
            lv_label_set_body_draw(setting_date_label[i], 1);
        }
        lv_label_set_text(setting_date_label[i], str[i]);
    }
}

uint8_t month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
void setting_date_key(size_t type, size_t value)
{
    if (type == TYPE_KEY_PRESS)
    {
        switch (value)
        {
        case KEY_UP:
            switch (g_select)
            {
            case 0:
                date.tm_year++;
                if (date.tm_year > 2120)
                    date.tm_year = 2120;
                if (date.tm_year % 4 == 0)
                    month_days[1] = 29;
                else
                    month_days[1] = 28;
                if (date.tm_mday > month_days[date.tm_mon])
                    date.tm_mday = month_days[date.tm_mon];
                break;
            case 1:
                date.tm_mon++;
                if (date.tm_mon > 11)
                    date.tm_mon = 0;
                if (date.tm_mday > month_days[date.tm_mon])
                    date.tm_mday = month_days[date.tm_mon];
                break;
            case 2:
                date.tm_mday++;
                if (date.tm_mday > month_days[date.tm_mon])
                    date.tm_mday = 1;
                break;
            case 3:
                date.tm_hour++;
                if (date.tm_hour > 23)
                    date.tm_hour = 0;
                break;
            case 4:
                date.tm_min++;
                if (date.tm_min > 59)
                    date.tm_min = 0;
                break;
            default:
                g_select = 0;
                break;
            }
            setting_date_refresh();
            break;
        case KEY_DOWN:
            switch (g_select)
            {
            case 0:
                date.tm_year--;
                if (date.tm_year < 1920 || date.tm_year > 2120)
                    date.tm_year = 1920;
                if (date.tm_year % 4 == 0)
                    month_days[1] = 29;
                else
                    month_days[1] = 28;
                if (date.tm_mday > month_days[date.tm_mon])
                    date.tm_mday = month_days[date.tm_mon];
                break;
            case 1:
                date.tm_mon--;
                if (date.tm_mon < 0 || date.tm_mon > 11)
                    date.tm_mon = 11;
                if (date.tm_mday > month_days[date.tm_mon])
                    date.tm_mday = month_days[date.tm_mon];
                break;
            case 2:
                date.tm_mday--;
                if (date.tm_mday < 1)
                    date.tm_mday = month_days[date.tm_mon];
                break;
            case 3:
                date.tm_hour--;
                if (date.tm_hour < 0 || date.tm_hour > 23)
                    date.tm_hour = 23;
                break;
            case 4:
                date.tm_min--;
                if (date.tm_min < 0 || date.tm_min > 59)
                    date.tm_min = 59;
                break;
            default:
                g_select = 0;
                break;
            }
            setting_date_refresh();
            break;
        case KEY_SELECT:
            g_select++;
            if (g_select > 4)
                g_select = 0;
            setting_date_refresh();
            break;
        case KEY_CANCEL:
            if (display_page(g_pPage) == RT_EOK)
                g_pPage = MENU_NONE_PAGE;
            break;
        case KEY_OK:
#ifdef RT_USING_RTC
            set_date(date.tm_year, date.tm_mon + 1, date.tm_mday);
            set_time(date.tm_hour, date.tm_min, date.tm_sec);
#endif
            if (display_page(g_pPage) == RT_EOK)
                g_pPage = MENU_NONE_PAGE;
            break;
        default:
            break;
        }
    }
}

void setting_date_init(int p_page)
{
    memcpy((void *)&date, (void *)g_app_info.date, sizeof(struct tm));
    date.tm_year += 1900;
    g_select = 0;
    if (date.tm_year % 4 == 0)
        month_days[1] = 29;
    else
        month_days[1] = 28;
    if (!setting_date)
    {
        if (g_pPage == MENU_NONE_PAGE)
            g_pPage = p_page;
        setting_date = lv_obj_create(lv_scr_act(), NULL);

        lv_obj_set_size(setting_date, SCREEN_W, SCREEN_H);
        lv_obj_set_style(setting_date, &page_common_style);
        lv_obj_align(setting_date, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        int y = 10;
        for (int i = 0; i < 5; i++)
        {
            setting_date_label[i] = lv_label_create(setting_date, NULL);
            lv_label_set_long_mode(setting_date_label[i], LV_LABEL_LONG_BREAK);
            lv_label_set_align(setting_date_label[i], LV_LABEL_ALIGN_LEFT);
            lv_obj_set_size(setting_date_label[i], SETTING_LABEL_W - 40, SETTING_LABEL_H);
            lv_obj_align(setting_date_label[i], NULL, LV_ALIGN_IN_TOP_MID, 0, y);
            y += SETTING_LABEL_H;
        }

        setting_date_refresh();
    }
}

void setting_date_deinit(void)
{
    if (setting_date)
    {
        lv_obj_del(setting_date);
        setting_date = NULL;
    }
}
