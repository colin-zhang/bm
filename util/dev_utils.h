#ifndef _DEV_UTILS_H
#define _DEV_UTILS_H

#include <time.h>

int dev_find_proc_pid(const char *proc_name);
int dev_test_pid(int pid);
time_t get_timespec_sec(void);
long dev_sys_uptime(void);

#endif

