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
    DEV_STATE_TOBE_MASTER,
};


struct board_info
{
    int session_id;
    int slot_id;
    int slot_type;
    int board_type;
    long uptime;  /* uptime of this board */
    char hw_version[32];
    char sw_version[32];
    long reg_time;
};
typedef struct board_info board_info_t;

struct dev_routine 
{
    dev_event_t *timer;
    int ifd;
    int ofd;
    int state;
    board_info_t board_info;
    void *td;
};
typedef struct dev_routine dev_routine_t;

struct master_info
{
    dev_routine_t *rt;
    dev_timer_ev_t *probe_timer;
    int max_board_num;
    int reg_board_num;
    struct board_info **boards;
    int max_master_board_num;
    int master_board_num;
    struct board_info **m_boards;
};
typedef struct master_info master_info_t;


struct io_info
{

};


typedef int slotid_array_t[16];

#endif
