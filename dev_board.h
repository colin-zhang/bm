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
    DEV_SATE_INIT = 0,
    DEV_STATE_MASTER,
    DEV_STATE_BACKUP,
    DEV_STATE_IO,
    DEV_STATE_IO_REG,
    DEV_SATE_IO_OFFLINE,
    DEV_STATE_MASTER_OFFLINE,
    DEV_STATE_IO_EXP,
    DEV_STATE_MASTER_EXP,
    DEV_STATE_TOBE_MASTER,
};

typedef struct board_info
{
    int session_id;
    int slot_id;
    int slot_type; // board_state
    int board_type;
    long uptime;  
    long uptime_m;  /* uptime of master board */
    char hw_version[32];
    char sw_version[32];
    int  timeout_chk; // 1 is timeout, it is offline
}board_info_t;

typedef struct dev_master_group
{
    int max_size;
    int count;
    board_info_t **member;
    int chief_index;
    int chiet_slotid;
    int update_flag;     // if 1 need fresh
}dev_master_group_t;

typedef struct dev_pkg_counter
{
    int slot_id;
    int type;
    int count;
}dev_pkg_counter_t;

typedef struct dev_routine 
{
    dev_event_t *timer;
    int ifd;
    int ofd;
    dev_event_t *board_api;
    board_info_t *self_info;
    dev_master_group_t *master_group;
    void *td;
}dev_routine_t;


board_info_t *dev_board_info_new(void);

void dev_board_info_update_state(board_info_t *to, board_info_t *from);

dev_routine_t *dev_board_rt_init(int *type);


int dev_master_group_probe_timeout_check(dev_master_group_t *dmg, int from);


int dev_master_group_add(dev_master_group_t *dmg, board_info_t * bif);
int dev_master_group_select_chief(dev_master_group_t *dmg);
int dev_master_group_chief_slotid(dev_master_group_t *dmg);

#endif
