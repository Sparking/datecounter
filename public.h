/* Project:Date Counter
 * Name:counter.h
 * Author:Sparking
 * Built time:2014-4-6 19:36
 */

#ifndef DATECOUNTER__PUBLIC_H__
#define DATECOUNTER__PUBLIC_H__

#include <inttypes.h>
//要使用到可移植类型及其格式化打印说明符

#define date_s SCNu16
#define date_p PRIu16
/* 使用#define宏与typedef
 * 是为了方便移植与修改
 */

typedef uint16_t date_t;
typedef uint8_t short_t;
/* 使用uint8来作为short_t类型,short_t
 * 类型的变量用来作小数组的索引最为合
 * 适,当然也可以用作其他用途.
 */

struct date{
    date_t year[2];
    date_t month[2];
    date_t day[2];
    };
/* 使用一个结构体,以储存所有的数据 */

extern date_t counter( struct date *);
extern void conversion( struct date *, const date_t);

#endif
