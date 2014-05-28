/* Project: Date Counter
 * Name: counter.c
 * Author: Sparking
 * Built time: 2014-4-6 16:40
 * 说明：该文件为本程序的核心。其中包括了counter()函数与conversion()函数
 *     counter()函数的作用————给定两个日期，算出两个日期直接按相差的天数。
 *     conversion()函数的作用————给定一个日期，再给定一个时间（多少天），
 *         函数将算出与该日期相差指定天数的日期（在该日期之后）。
 */


#include "public.h"
#define spec_year(x) (((x % 4 == 0) && (x % 100 != 0))||(x % 400 == 0))

date_t month_D[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/*   swap()函数用于交换数据，可以根据需要将其改为内联函数
 *   该函数只限于在本文件中使用，以防引起混乱。
 */
static void swap( date_t * a, date_t * b)
{
    date_t i;
    i = *a;
    *a = *b;
    *b = i;
}

date_t counter( struct date *d)   /* 接受一个储存两个完整日期的结构体的地址作为参数*/
{
    short_t i;
    date_t sum = 0;

    if( d->year[0] == d->year[1]) /* 由于要考虑到效率，先判断两个日期是否在同一年内 */
    {
        if( spec_year(d->year[0]))   /*若年份是否为闰年，二月份改为29天，否则为28天 */
            month_D[1] = 29;
        else
            month_D[1] = 28;
        if( d->month[0] == d->month[1])     /*若月份相同，则只需要计算两天之间的天数*/
        {
            if( d->day[0] >= d->day[1])      /*   因为date_t为无符号的整数，所以要  */
                sum = d->day[0] - d->day[1]; /*   注意减法运算可能会带来的问题。    */
            else
                sum = d->day[1] - d->day[0];
        }
        else
        {
            if( d->month[0] > d->month[1])   /*   若月份不同，则对换月份与号数的值  */
            {
                swap( &d->month[0], &d->month[1]);
                swap( &d->day[0], &d->day[1]);
            }
            for( i = d->month[0] - 1; i < d->month[1] - 1;)
                sum += month_D[ i++];
            sum = sum + d->day[1] - d->day[0];
            //将天数逐月累加起来，加到第二个日期前一个月的天数为止。将所得到的和加上
            //第二个日期中的天数，最后减去第一个日期的号数才能得到正确结果。
        }
    }
    else                          /*   若两个日期不在同一年，则判断是否要交换顺序   */
    {                             /*先判断两个日期的先后顺序，日期应该按时间顺序排布*/
        if( d->year[0] > d->year[1])   /*  若日期不按时间顺序排布，则交换两个日期   */
        {
            swap( &d->year[0], &d->year[1]);
            swap( &d->month[0], &d->month[1]);
            swap( &d->day[0], &d->day[1]);
        }

        if( spec_year(d->year[0]))   /*若年份是否为闰年，二月份改为29天，否则为28天 */
            month_D[1] = 29;
        for( i = d->month[0] - 1; i < 12;)
            sum += month_D[i++];
        sum -= d->day[0];

        while( ++ d->year[0] < d->year[1])
        {
            if( spec_year(d->year[0]))
                sum += 366;
            else
                sum += 365;
        }

        if( spec_year(d->year[1]))
            month_D[1] = 29;
        else
         month_D[1] = 28;
        for( i = 0; i < d->month[1] - 1; i++)
            sum += month_D[i];
        sum += d->day[1];
    }

    return sum;
}


void conversion( struct date *d, const date_t days)
{
    date_t dayx = days;
    date_t yi, mi, di;
    yi = d->year[0], mi = d->month[0], di = d->day[0];

    while( mi <= 12)
    {
        if( spec_year(yi))
            month_D[1] = 29;
        else
            month_D[1] = 28;
        if( !( mi == d->month[0] && yi == d->year[0]))
            di = 0;
        if( dayx <= month_D[mi - 1] - di)
        {
            dayx += di;
            break;
        }
        else
        {
            dayx = dayx - ( month_D[mi - 1] - di);
            mi++;
            if(mi == 13)
            {
                yi++;
                mi = 1;
            }
        }
    }

    d->year[1] = yi;
    d->month[1] = mi;
    d->day[1] = dayx;
}
