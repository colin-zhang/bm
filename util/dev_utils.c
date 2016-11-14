#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include "dev_utils.h"
#include <time.h>

#define DEV_ONE_SECOND  1000000000
#define DEV_ONE_MSECOND 1000000
#define DEV_ONE_VSECOND 1000

long 
dev_sys_uptime(void)
{
    struct sysinfo info;    
    if (sysinfo(&info)) {
        fprintf(stderr, "Failed to get sysinfo, errno:%u, reason:%s\n", errno, strerror(errno));
        return -1;
    }
    return info.uptime;
}

time_t
get_timespec_sec(void)
{
   struct timespec curr = {0,};
   clock_gettime(CLOCK_MONOTONIC, &curr);
   return curr.tv_sec;
}


uint32_t SwapShort(uint16_t a)
{
  a = ((a & 0x00FF) << 8) | ((a & 0xFF00) >> 8);
  return a;
}

uint32_t SwapWord(uint32_t a)
{
  a = ((a & 0x000000FF) << 24) |
      ((a & 0x0000FF00) <<  8) |
      ((a & 0x00FF0000) >>  8) |
      ((a & 0xFF000000) >> 24);
  return a;
}

uint64_t SwapDWord(uint64_t a)
{
  a = ((a & 0x00000000000000FFULL) << 56) | 
      ((a & 0x000000000000FF00ULL) << 40) | 
      ((a & 0x0000000000FF0000ULL) << 24) | 
      ((a & 0x00000000FF000000ULL) <<  8) | 
      ((a & 0x000000FF00000000ULL) >>  8) | 
      ((a & 0x0000FF0000000000ULL) >> 24) | 
      ((a & 0x00FF000000000000ULL) >> 40) | 
      ((a & 0xFF00000000000000ULL) >> 56);
  return a;
}


int 
dev_timespec_cmp(struct timespec *ts1, struct timespec *ts2)
{
    if (ts1->tv_sec > ts2->tv_sec) {
        return 1;
    } else if (ts1->tv_sec < ts2->tv_sec) {
        return -1;
    }
    else {
        if (ts1->tv_nsec > ts2->tv_nsec) {
            return 1001;
        } else if (ts1->tv_nsec < ts2->tv_nsec) {
            return -1001;
        }
        else {
            return 0;
        }
    }
}



void
dev_timespec_add(struct timespec *ts1, struct timespec *ts2) 
{
    ts1->tv_sec += ts2->tv_sec;
    ts1->tv_nsec += ts2->tv_nsec;

    if (ts1->tv_nsec >= DEV_ONE_SECOND) {
        ts1->tv_nsec %= DEV_ONE_SECOND;
        ts1->tv_sec++;
    }
}

int
dev_timespec_minus(struct timespec *ts1, struct timespec *ts2) 
{
    int ret;

    ret = dev_timespec_cmp(ts1, ts2);
    if (ret > 1000) {
        ts1->tv_sec = 0;
        ts1->tv_nsec -= ts2->tv_nsec; 
    } else if (ret > 0) {
        if (ts2->tv_nsec > ts1->tv_nsec) {
            ts1->tv_sec--;
            ts1->tv_nsec += DEV_ONE_SECOND - ts2->tv_nsec;
        } else {
            ts1->tv_nsec -= ts2->tv_nsec;
        }
        ts1->tv_sec -= ts2->tv_sec;
    } else if (ret == 0) {
        ts1->tv_sec = 0;
        ts1->tv_nsec = 0;
    } else {
        return -1;
    }
    return 0;
}

//if (__BYTE_ORDER == __LITTLE_ENDIAN)
//if (__BYTE_ORDER == __BIG_ENDIAN)

/*CLOCK_REALTIME
System-wide realtime clock. Setting this clock requires appropriate privileges.
CLOCK_MONOTONIC
Clock that cannot be set and represents monotonic time since some unspecified starting point.
int clock_gettime(clockid_t clk_id, struct timespec *tp);
int clock_settime(clockid_t clk_id, const struct timespec *tp);*/

