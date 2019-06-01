/* Project: Date Counter
 * Name: main.c
 * Author: Sparking
 * Mail: liujiaying_7@126.com
 * Built time: 2014-5-5 20:25
 * Change:0.0.2
 */

/*   说明：
 *       该程序是用于计算两个日期之间相差的天数。
 *   若没有参数，则程序进入命令行界面进行输入并计算
 *   若给定参数，则程序直接输出结果。
 *   以下为参数选项及其说明：
 *   -h,--help：   列出帮助说明
 *   -v,--version：显示版本号
 *   -a,--add      该选项后接一个整数，即天数
 *   -f,--from：   该选项后接第一个日期（例如：2014 1 1）
 *   -t,--to：     该选项后接第二个日期（同 -f,--from）
 *
 *   注意：当选用-t或--to参数时，可以不用跟上-f或
 *        --from选项，因为此时程序会将第一个日期默认为系
 *        统当前日期。
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "date_counter.h"
#include "version.h"
#define IS(X) (isargv(X,argv[i]))

static bool isargv( const char *stringA, const char *stringB)
/* This function aims to compare
 * two strings.I don't use strcmp()
 * to make program simple and light.
 */
{
    while( *stringA )
        if( *stringA ++ != *stringB++)
            return false;
    if( *stringB)
        return false;
    return true;
}

static int getfirst(void)
/* This function will get the
 * first character of one-line
 * input and will return it.
 */
{
    int ch;
    ch = getchar();
    while( isspace(ch))
        ch = getchar();
    while( getchar() != '\n')
        continue;
    return ch;
}

int main(int argc, char *argv[])
{
    int ch;
    uint32_t sum;
    uint32_t days;
    uint16_t i;

    bool need_help   = false;
    bool show_ver    = false;
    bool has_first   = false;
    bool has_second  = false;
    bool convert_yes = false;

    struct date d[2];
    time_t ltime;
    struct tm *today;

    time(&ltime);
    today = localtime(&ltime);

    if (argc == 1) {
        fprintf(stdout, "There two models for use:\n"
                "a. give a date and an integer as days to compute a new date\n"
                "b. give two dates to count the days between dates\n"
                "q. that means quit.\nEnter your choice:");
        while ((ch = getfirst()) != 'q') {
           if (ch == 'a') {
                fprintf(stdout,  "Enter the date:");
                if (scanf("%hu%hu%hu", &d[0].year, &d[0].month, &d[0].day) != 3) {
                    fprintf(stderr, "Wrong format!!!\n");
                    exit(EXIT_FAILURE);
                }
                fprintf(stdout, "Enter the days:");
                if (scanf("%u", &days) != 1) {
                     fprintf(stderr, "Wrong format!!!\n");
                     exit(EXIT_FAILURE);
                }
                date_counter_compute_date(&d[1], &d[0], days);
                fprintf(stdout, "New date is %hu %hu %hu\n", d[1].year, (uint16_t)d[1].month, (uint16_t)d[1].day);
           } else if (ch == 'b') {
                for (i = 0; i < 2 ; i++) {
	                if (i == 0)
	                    printf("Please input the first date:");
	                else
	                    printf("Please input the second date:");
	                if( scanf("%hu%hu%hu", &d[i].year, &d[i].month, &d[i].day) != 3) {
                        fprintf(stderr, "Wrong input!!!\n");
                        exit(EXIT_FAILURE);
                    }
               }
               sum = date_counter_compute_days(&d[0], &d[1]);
               fprintf(stdout, "The days between two dates are %u.\n", sum);
           } else {
               fprintf(stderr, "recognize only a, b and q.\n");
           }
           fprintf(stdout, "Enter your choice again:");
        }
    } else {
        for (i = 1; i < argc ; i++) {
            if (IS("-")) {
            	fprintf(stderr, "\"%s\" is invaild\n", argv[i]);
            	exit(EXIT_FAILURE);
            } else if (IS("-h") || IS("--help")) {
                need_help = true;
            } else if (IS("-v") || IS("--version")) {
            	show_ver = true;
            } else if (IS("-f") || IS("--from")) {
            	if (argc >= i + 3){
            	   d[0].year = atoi(argv[++i]);
            	   d[0].month = atoi(argv[++i]);
            	   d[0].day = atoi(argv[++i]);
            	   has_first = true;
            	} else {
            	    fprintf(stderr, "The option %s has too few arguments.\n", argv[i]);
            	    exit(EXIT_FAILURE);
            	}
            } else if (IS("-t") || IS("--to")) {
            	if (argc >= i + 3) {
            	    d[1].year = atoi(argv[++i]);
            	    d[1].month = atoi(argv[++i]);
            	    d[1].day = atoi(argv[++i]);
            	    has_second = true;
                } else {
            	    fprintf(stderr, "The option %s has too few arguments.\n", argv[i]);
            	    exit(EXIT_FAILURE);
                }
            } else if (IS("-a") || IS("--add")) {
                if( argc >= i + 1) {
                     days = atoi(argv[++i]);
                     convert_yes = true;
                } else {
                    exit(EXIT_FAILURE);
                }
            } else {
            	fprintf(stderr, "unrecognized option \"%s\"\n", argv[i]);
                fprintf(stderr, "usage: %s [options]\n%s", argv[0], HELP);
            	exit(EXIT_FAILURE);
            }
        }
        if (!has_first) {
            d[0].year = (uint16_t)1900+today->tm_year;
            d[0].month = (uint8_t)today->tm_mon + 1;
            d[0].day = (uint8_t)today->tm_mday;
        }

        if (need_help) {
            fprintf(stdout, "usage: %s [options]\n%s\n", argv[0], HELP);
            exit(EXIT_SUCCESS);
        } else if (show_ver) {
            puts( PRGNAME"  "VERSION);
            fprintf( stdout, "Built by %s.\n", AUTHORS);
            exit(EXIT_SUCCESS);
        } else if (has_second && !convert_yes) {
            sum = date_counter_compute_days(d, d + 1);
            fprintf(stdout,"%u", sum);
            exit(EXIT_SUCCESS);
        }
        else if( convert_yes && !has_second){
            date_counter_compute_date(&d[1], &d[0], days);
            fprintf(stdout, "%hu-%hu-%hu\n", d[1].year, (uint16_t)d[1].month, (uint16_t)d[1].day);
            exit(EXIT_SUCCESS);
        } else {
            fprintf( stderr, "Invaild operation!\n");
            exit( EXIT_FAILURE);
        }
    }

	return 0;
}
