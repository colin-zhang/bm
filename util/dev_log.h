#ifndef __DEVD_DEV_LOG_H__
#define __DEVD_DEV_LOG_H__

#include <stdio.h>

#define LOG_DEBUG 1

#ifdef LOG_DEBUG
        #define DEBUG_PRINT(fmt, args...) \
            do { \
                fprintf(stderr, "DBG:%s(%d)-%s: "fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##args); \
                log_printf("DBG:%s(%d)-%s: "fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##args); \
                fflush(stderr); \
            }while(0)
#else
        #define DEBUG_PRINT(fmt, args...)  fprintf(stderr, "DBG:%s(%d)-%s: "fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##args);
#endif

void log_printf(const char *format, ...);

#endif