#ifndef _DEV_MASTER_H
#define _DEV_MASTER_H

#include "core/dev_event.h"

struct master_info;
typedef struct master_info master_info_t;


dev_event_t * dev_master_creat(void *data);

#endif