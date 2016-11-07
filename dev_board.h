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
typedef int slotid_array_t[16];

enum board_state
{
    DEV_SATE_INIT,
    DEV_STATE_MASTER,
    DEV_STATE_BACKUP,
    DEV_STATE_IO,
    DEV_STATE_IO_REG,
    DEV_STATE_IO_EXP,
    DEV_STATE_MASTER_EXP,
    DEV_STATE_TOBE_MASTER,
};

typedef struct board_info
{
    int session_id;
    int slot_id;
    int slot_type;
    int board_type;
    long uptime;  /* uptime of this board */
    char hw_version[32];
    char sw_version[32];
}board_info_t;

typedef struct dev_master_group
{
    int max_size;
    int count;
    int chief_index;
    int update_flag;     // if 1 need fresh
    board_info_t **member;
}dev_master_group_t;

typedef struct dev_routine 
{
    dev_event_t *timer;
    int ifd;
    int ofd;
    int state;
    board_info_t *self_info;
    dev_master_group_t *master_group;
    void *td;
}dev_routine_t;


dev_routine_t *dev_board_rt_init(int *type);



#endif
