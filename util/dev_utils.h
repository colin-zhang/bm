#ifndef _DEV_UTILS_H
#define _DEV_UTILS_H

#include <time.h>

time_t get_timespec_sec(void);
long dev_sys_uptime(void);


uint64_t dev_htonll(uint64_t a);
uint64_t dev_ntohll(uint64_t a);

#endif

