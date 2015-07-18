/**
 * FILE     : log.h
 * DESC     : log output
 * AUTHOR   : v0.1 written by Alex Stocks
 * DATE     : on June 13, 2015
 * LICENCE  : GPL 2.0
 * MOD      :
 **/

#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

#define pline(fmt, ...)     printf(fmt"\n", ##__VA_ARGS__)
#define pinfo(fmt, ...)     \
    printf("%s-%s-%d: "fmt"\n", ((char*)__FILE__), \
            (char*)__FUNCTION__, (int)__LINE__, ##__VA_ARGS__)
#define perr(fmt, ...)     \
    printf("FATAL: %s-%s-%d: error: "fmt"\n", \
            (char*)__FILE__, (char*)__FUNCTION__, (int)__LINE__, ##__VA_ARGS__)

#define FATAL  perr
#define DEBUG  pinfo

#endif

