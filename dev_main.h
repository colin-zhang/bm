#ifndef _DEV_MAIN_H
#define _DEV_MAIN_H

#include "./def/dev_def.h"
#include "core/dev_event_loop.h"
#include "core/dev_event_timer.h"
#include "core/dev_event.h"
#include "core/dev_signalfd.h"
#include "util/dev_udp.h"
#include "util/dev_if_so.h"
#include "util/dev_utils.h"

#define MAX_BOARD_NUM 14

enum board_state
{
    DEV_STATE_INIT,
    DEV_STATE_MASTER,
    DEV_STATE_BACKUP,
    DEV_STATE_IO,
    DEV_STATE_REG,
    DEV_STATE_EXP,
};

struct dev_routine 
{
    dev_event_t *timer;
    time_t start_time;
    int ifd;
    int ofd;
    int state;
    int slot_id;
    int slot_type;
    void *td;
};

typedef struct dev_routine dev_routine_t;

struct board_info
{
    int session_id;
    int slot_id;
    long reg_time;
    long uptime;
    char hw_version[32];
    char sw_version[32];
};

struct master_info
{
    dev_routine_t *rt;
    int max_board_num;
    int reg_board_num;
    dev_timer_ev_t *probe_timer;
    struct board_info **boards;
};

struct io_info
{

};


typedef struct board_info board_info_t;
typedef struct master_info master_info_t;

typedef int slotid_array_t[16];

#endif
