#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include "dev_utils.h"
#include <time.h>

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
