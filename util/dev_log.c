#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

#include "dev_log.h"

static pthread_mutex_t log_mut = PTHREAD_MUTEX_INITIALIZER;
void
log_printf(const char *format, ...)
{
    FILE *f;
    va_list args;
    va_start(args, format);

    pthread_mutex_lock(&log_mut);
    if ((f = fopen("/var/log/dev2d_log", "a+")) != NULL) {
        vfprintf(f, format, args);
        fflush(f);
        fclose(f);
    }
    va_end(args);
    pthread_mutex_unlock(&log_mut);
}
