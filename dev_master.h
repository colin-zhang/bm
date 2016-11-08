#ifndef _DEV_MASTER_H
#define _DEV_MASTER_H

#include "core/dev_event.h"

struct master_info
{
    dev_routine_t *rt;
    dev_timer_ev_t *probe_timer;
    dev_timer_ev_t *check_timer;
    dev_timer_ev_t *vote_timer;

    int max_board_num;
    int reg_board_num;
    board_info_t **boards;
};

typedef struct master_info master_info_t;

dev_event_t * dev_master_creat(void *data);

#endif