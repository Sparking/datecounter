/* Project:Date Counter
 * Name:counter.h
 * Author:Liu Jiaying
 * Built time:2014-4-6 19:36
 */

#ifndef DATECOUNTER_PUBLIC_H_
#define DATECOUNTER_PUBLIC_H_

#include <inttypes.h>
#include <stdlib.h>

#define date_s SCNu16
#define date_p PRIu16 

typedef uint16_t date_t;
typedef uint8_t short_t;
struct date{
    date_t year[2];
    date_t month[2];
    date_t day[2];
    };


extern date_t counter(struct date *);
extern void conversion(struct date *,const date_t);

#endif
