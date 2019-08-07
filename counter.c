/* Project: Date Counter
 * Name: counter.c
 * Author: Sparking
 * Built time: 2014-4-6 16:40
 * 说明：该文件为本程序的核心。其中包括了counter()函数与conversion()函数
 *     counter()函数的作用————给定两个日期，算出两个日期直接按相差的天数。
 *     conversion()函数的作用————给定一个日期，再给定一个时间（多少天），
 *         函数将算出与该日期相差指定天数的日期（在该日期之后）。
 */
#include <time.h>
#include <string.h>
#include "date_counter.h"

#define spec_year(x) (((x % 4 == 0) && (x % 100 != 0))||(x % 400 == 0))

static uint8_t __month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define swap_t(a, b, type)  \
    do { \
        type temp; \
        temp = a; \
        a = b; \
        b = temp; \
    } while(0)

void date_counter_get_current_date(struct date *restrict d)
{
    if (d != NULL) {
        time_t ltime;
        struct tm *today;

        time(&ltime);
        today = localtime(&ltime);
        d->year = (uint16_t)1900 + today->tm_year;
        d->month = (uint8_t)today->tm_mon + 1;
        d->day = (uint8_t)today->tm_mday;
    }
}

uint32_t date_counter_compute_days(const struct date *restrict date_a,
    const struct date *restrict date_b)
{
    struct date date[2];
    uint32_t sum;
    uint32_t i;

    sum = 0;
    memcpy(&date[0], date_a, sizeof(struct date));
    memcpy(&date[1], date_b, sizeof(struct date));
    if (date[0].year == date[1].year) {
        if (spec_year(date[0].year))
            __month_days[1] = 29;
        else
            __month_days[1] = 28;

        if (date[0].month == date[1].month) {
            if( date[0].day >= date[1].day)
                sum = date[0].day - date[1].day;
            else
                sum = date[1].day - date[0].day;
        } else {
            if (date[0].month > date[1].month) {
                swap_t(date[0].month, date[1].month, uint8_t);
                swap_t(date[0].day, date[1].day, uint8_t);
            }
            for (i = date[0].month - 1; i < date[1].month - 1; i++)
                sum += __month_days[i];
            sum = sum + date[1].day - date[0].day;
        }
    } else {
        if (date[0].year > date[1].year) {
            swap_t(date[0].year, date[1].year, uint16_t);
            swap_t(date[0].month, date[1].month, uint8_t);
            swap_t(date[0].day, date[1].day, uint8_t);
        }

        if (spec_year(date[0].year))
            __month_days[1] = 29;
        else
            __month_days[1] = 28;
        for (i = date[0].month - 1; i < 12; i++)
            sum += __month_days[i];
        sum -= date[0].day;

        while (++date[0].year < date[1].year) {
            if (spec_year(date[0].year))
                sum += 366;
            else
                sum += 365;
        }

        if (spec_year(date[1].year))
            __month_days[1] = 29;
        else
            __month_days[1] = 28;
        for( i = 0; i < date[1].month - 1; i++)
            sum += __month_days[i];
        sum += date[1].day;
    }

    return sum;
}

void  date_counter_compute_date(struct date *restrict dst_date,
    const struct date *restrict src_date, const uint32_t days)
{
    uint32_t dayx;
    uint16_t yi;
    uint8_t mi, di;

    dayx = days;
    yi = src_date->year;
    mi = src_date->month;
    di = src_date->day;
    while (mi <= 12) {
        if (spec_year(yi))
            __month_days[1] = 29;
        else
            __month_days[1] = 28;
        if (!( mi == src_date->month && yi == src_date->year))
            di = 0;
        if (dayx <= __month_days[mi - 1] - di) {
            dayx += di;
            break;
        } else {
            dayx = dayx - ( __month_days[mi - 1] - di);
            mi++;
            if(mi == 13) {
                yi++;
                mi = 1;
            }
        }
    }

    dst_date->year = yi;
    dst_date->month = mi;
    dst_date->day = dayx;
}
