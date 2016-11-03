#include "./def/dev_def.h"
#include "core/dev_event_loop.h"
#include "core/dev_event.h"
#include "core/dev_event_timer.h"
#include "core/dev_signalfd.h"
#include "util/dev_udp.h"
#include "util/dev_if_so.h"
#include "util/dev_utils.h"
#include "util/dev_pid.h"

#include "dev_board.h"

#define MAX_MASTER_NUM (2)


board_info_t *
dev_board_info_new()
{
    board_info_t *bif = calloc(1, sizeof(board_info_t));
    return bif;
}

dev_master_group_t *
dev_master_group_creat(int num)
{
    dev_master_group_t *dmg = NULL;
    dmg = calloc(1, sizeof(dev_master_group_t));
    if (dmg == NULL) return dmg;
    dmg->max_size = num;
    dmg->chief_index = -1;
    dmg->update_flag = 0;
    dmg->count = 0;
    dmg->member = calloc(num, sizeof(board_info_t *));
    if (dmg->member == NULL) {
        free(dmg);
        return NULL;
    }
    return dmg;
}

int 
dev_master_group_search_by_slot(dev_master_group_t *dmg, int slot_id)
{
    int i;
    for (i = 0; i < dmg->count; i++) {
        if (dmg->member[i]->slot_id == slot_id) {
            return i;
        }
    }
    return -1;
}

/* DEV_STATE_TOBE_MASTER */
int 
dev_master_group_search_by_boardtype(dev_master_group_t *dmg, int type, int *them)
{
    int i = 0, j = 0;
    for (i = 0, j = 0; i < dmg->count; i++) {
        if (dmg->member[i]->board_type == type) {
            them[j++] = i;
        }
    }
    return j;
}

int
dev_master_group_set_chief(dev_master_group_t *dmg, int index)
{
    int i = 0;

    dmg->chief_index = index;
    dmg->update_flag = 0;

    for (i = 0; i < dmg->count; i++) {
        if (i == index) {
            dmg->member[i]->board_type = DEV_STATE_MASTER;
        } else {
            dmg->member[i]->board_type = DEV_STATE_BACKUP;
        }
    }
    return 0;
}

int
dev_master_group_select(dev_master_group_t *dmg, int *them, int num)
{
    int i, index = 0, base_index = 0;

    base_index = them[0];

    for (i = 1; i < dmg->count && i < num; i++) {
        index = them[i];
        if (dmg->member[index]->uptime > dmg->member[base_index]->uptime) {
            base_index = index;
        } else if (dmg->member[index]->uptime == dmg->member[base_index]->uptime) {
            if (dmg->member[index]->slot_id > dmg->member[base_index]->slot_id) {
                base_index = index;
            }
        }
    }
    return base_index;
}


int
dev_master_group_select_chief(dev_master_group_t *dmg)
{
    int index = 0, i = 0;
    int num = 0;
    int select_indexs[MAX_MASTER_NUM] = {0};

    num = dev_master_group_search_by_boardtype(dmg, DEV_STATE_TOBE_MASTER, select_indexs);
    if (num == 0) {
        num = dmg->count;
        for (i = 0; i < num; i++) {
            select_indexs[i] = i;
        }
    }
    index = dev_master_group_select(dmg, select_indexs, num);
    dev_master_group_set_chief(dmg,  index);
    return index;
}

int 
dev_master_group_add(dev_master_group_t *dmg, board_info_t * bif)
{
    int index = -1;

    index = dev_master_group_search_by_slot(dmg, bif->slot_id);
    if (index >= 0) {
        return index;
    }

    if (dmg->count == dmg->max_size) {
        return -1;
    }

    dmg->member[dmg->count] = bif;
    dmg->count++;
    if (dmg->count) {
        dmg->update_flag = 1;
    }
    return 0;
}

dev_routine_t *
dev_board_rt_init(int *type)
{
    dev_routine_t *rt = NULL;
    rt = calloc(1, sizeof(dev_routine_t));
    if (rt == NULL) {
        exit(-1);
    }
    rt->timer = dev_event_timer_creat(50, rt);

    rt->self_info = dev_board_info_new();
    rt->master_group = dev_master_group_creat(MAX_MASTER_NUM);    

    dev_self_board_info(rt->self_info);
    dev_master_group_add(rt->master_group, rt->self_info);

    switch (rt->self_info->slot_type) {
        case DEV_STATE_MASTER:
        case DEV_STATE_BACKUP:
        case DEV_STATE_MASTER_EXP:
        case DEV_STATE_TOBE_MASTER:
            rt->self_info->slot_type = DEV_STATE_BACKUP;
            break;
        case DEV_STATE_IO:
        case DEV_STATE_IO_REG:
        case DEV_STATE_IO_EXP:
            rt->self_info->slot_type = DEV_STATE_IO;
            break;
    }
    *type = rt->self_info->slot_type;
    return rt;
}

int 
dev_getenv_int(const char *var)
{
    char *env_ptr = NULL;
    env_ptr = getenv(var);
    if (env_ptr) {
        return atoi(env_ptr);
    } else {
        fprintf(stderr, "%s\n", "can not get env");
    }
    return 0;
}

int 
dev_self_board_info(board_info_t *bif)
{
    bif->slot_id = dev_getenv_int("slotid");
    printf("slot_id = %d\n", bif->slot_id);
    bif->session_id = 0;
    bif->board_type = 0x001;
    bif->slot_type = DEV_STATE_BACKUP;
    bif->uptime = dev_sys_uptime();
    snprintf(bif->hw_version, sizeof(bif->hw_version), "%s", "v111");
    snprintf(bif->hw_version, sizeof(bif->sw_version), "%s", "v_111");
    return 0;
}



