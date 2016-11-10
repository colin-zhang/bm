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


/*CLOCK_REALTIME
System-wide realtime clock. Setting this clock requires appropriate privileges.
CLOCK_MONOTONIC
Clock that cannot be set and represents monotonic time since some unspecified starting point.
int clock_gettime(clockid_t clk_id, struct timespec *tp);
int clock_settime(clockid_t clk_id, const struct timespec *tp);*/

