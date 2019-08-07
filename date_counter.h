/* Project:Date Counter
 * Name:counter.h
 * Author:Sparking
 * Built time:345ERSTDFXC VL,M .014-4-6 19:36
 */

#ifndef __DATE_COUNTER_H_
#define __DATE_COUNTER_H_

#include <stdio.h>
#include <inttypes.h>

struct date {
    uint16_t year;
    uint8_t month;
    uint8_t day;
};


static inline void date_counter_string_to_date(const char *restrict str,
    struct date *restrict d) {
    unsigned short a, b, c;
    sscanf(str, "%hu-%hu-%hu", &a, &b, &c);
    d->year = (uint16_t)a;
    d->month = (uint8_t)b;
    d->day = (uint8_t)c;
}
extern void date_counter_get_current_date(struct date *restrict d);
extern uint32_t date_counter_compute_days(const struct date *restrict date_a,
    const struct date *restrict date_b);
extern void  date_counter_compute_date(struct date *restrict dst_date,
    const struct date *restrict src_date, const uint32_t days);

#endif
