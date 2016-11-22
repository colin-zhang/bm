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
#include "dev_board_api.h"

#define MAX_MASTER_NUM (4)

extern void dev_protocol_init_boardinfo(board_info_t *bif);
extern int dev_protocol_port(void);
static int dev_self_board_info_init(board_info_t *bif);

board_info_t *
dev_board_info_new(void)
{
    board_info_t *bif = calloc(1, sizeof(board_info_t));
    return bif;
}

void 
dev_board_info_update_state(board_info_t *to, board_info_t *from) 
{
    to->slot_type = from->slot_type;
    to->timeout_chk = from->timeout_chk;
    to->uptime = from->uptime;
}

dev_master_group_t *
dev_master_group_creat(int num)
{
    dev_master_group_t *dmg = NULL;
    dmg = calloc(1, sizeof(dev_master_group_t));
    if (dmg == NULL) return NULL;
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

void 
dev_master_group_print(dev_master_group_t *dmg)
{
    int i;

    printf("num=%d chief_index=%d\n", dmg->count, dmg->chief_index);

    for (i = 0; i < dmg->count; i++) {
        printf(
            "slotid=%d"
            ", slotype=%d"
            ", uptime=%ld"
            "  \n", 
            dmg->member[i]->slot_id,
            dmg->member[i]->slot_type,
            dmg->member[i]->uptime
            );
    }
}

static int 
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
static int 
dev_master_group_search_by_slottype(dev_master_group_t *dmg, int type, int *them)
{
    int i = 0, j = 0;
    for (i = 0, j = 0; i < dmg->count; i++) {
        if (dmg->member[i]->slot_type == type) {
            them[j++] = i;
        }
    }
    return j;
}

static int
dev_master_group_set_chief(dev_master_group_t *dmg, int index)
{
    int i = 0;
    for (i = 0; i < dmg->count; i++) {
        dmg->member[i]->slot_type = DEV_STATE_BACKUP;
    }
    if (index >=0 && index < dmg->count) {
        dmg->member[index]->slot_type = DEV_STATE_MASTER;
        dmg->chief_index = index;
        dmg->update_flag = 0;
    }
    return 0;
}
/* 
dev_master_uptime_compare
a > b return 1
*/
static inline int dev_muc(long a, long b)
{
    if ( a > b + 10) {
        return 1;
    } else {
        return 0;
    }
}

static int
dev_master_group_select(dev_master_group_t *dmg, int *them, int num)
{
    int i, index = 0, base_index = 0;

    base_index = them[0];
    for (i = 1; i < dmg->count && i < num; i++) {
        index = them[i];
        printf("a=%ld, b=%ld\n", dmg->member[index]->uptime, dmg->member[base_index]->uptime);
        if (dev_muc(dmg->member[index]->uptime, dmg->member[base_index]->uptime)) {
            base_index = index;
        } else if (dmg->member[index]->uptime == dmg->member[base_index]->uptime) {
            if (dmg->member[index]->slot_id < dmg->member[base_index]->slot_id) {
                base_index = index;
            }
        }
    }
    return base_index;
}

int
dev_master_group_chief_slotid(dev_master_group_t *dmg)
{
    return dmg->member[dmg->chief_index]->slot_id;
}

int
dev_master_group_select_chief(dev_master_group_t *dmg)
{
    int index = 0, i = 0;
    int num = 0;
    int select_indexs[MAX_MASTER_NUM] = {0};

    num = dev_master_group_search_by_slottype(dmg, DEV_STATE_TOBE_MASTER, select_indexs);
    if (num == 0) {
        num = dev_master_group_search_by_slottype(dmg, DEV_STATE_MASTER, select_indexs); 
        if (num == 0) {
            num = dev_master_group_search_by_slottype(dmg, DEV_STATE_BACKUP, select_indexs);
        }
    }
    index = dev_master_group_select(dmg, select_indexs, num);
    dev_master_group_set_chief(dmg, index);
    dmg->chiet_slotid = dev_master_group_chief_slotid(dmg);
    printf("index = %d, dmg->count = %d, chief slot_id = [%d]\n", index, dmg->count, dmg->chiet_slotid);
    return index;
}

int 
dev_master_group_probe_timeout_check(dev_master_group_t *dmg, int from)
{
    int i = 0;
    for (i = from; i < dmg->count; i++) {
        if (dmg->member[i]->timeout_chk) {
            dmg->member[i]->slot_type = DEV_STATE_MASTER_OFFLINE;
            return 1;
        } else {
            dmg->member[i]->timeout_chk = 1;
        }
    }
    return 0;
}

int 
dev_master_group_add(dev_master_group_t *dmg, board_info_t * bif)
{
    int index = -1;

    index = dev_master_group_search_by_slot(dmg, bif->slot_id);
    if (index >= 0) {
        dev_board_info_update_state(dmg->member[index], bif);
        return index;
    }
    
    if (dmg->count == dmg->max_size) {
        return -1;
    }

    if (dmg->count == 0 && bif->slot_type == DEV_STATE_BACKUP) {
        dmg->member[0] = bif;
    } else {
        board_info_t *ptr = dev_board_info_new();
        memcpy(ptr, bif, sizeof(board_info_t));
        dmg->member[dmg->count] = ptr;
    }
 
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
    rt->timer = dev_event_timer_creat(20, rt);
    rt->self_info = dev_board_info_new();
    rt->master_group = dev_master_group_creat(MAX_MASTER_NUM);    

    dev_self_board_info_init(rt->self_info);
    dev_protocol_init_boardinfo(rt->self_info);

    rt->ifd = dev_udp_port_creat(rt->self_info->slot_id, dev_protocol_port());
    rt->ofd = dev_udp_client_creat();
    rt->board_api = dev_board_api_init(rt);
    
    switch (rt->self_info->slot_type) {
        case DEV_STATE_MASTER:
        case DEV_STATE_BACKUP:
        case DEV_STATE_MASTER_EXP:
        case DEV_STATE_TOBE_MASTER:
            rt->self_info->slot_type = DEV_STATE_BACKUP;
            dev_master_group_add(rt->master_group, rt->self_info);
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
        return strtoul(env_ptr, NULL, 0);;
    } else {
        fprintf(stderr, "%s\n", "can not get env");
    }
    return 0;
}

static int 
dev_self_board_info_init(board_info_t *bif)
{
    bif->slot_id = dev_getenv_int("slotid");
    printf("slot_id = %d\n", bif->slot_id);
    bif->session_id = 0;
    bif->board_type = dev_getenv_int("boardtype");
    printf("boardtype = %x\n", bif->board_type);
    bif->slot_type = dev_getenv_int("slottype");;
    printf("slot_type = %d\n", bif->slot_type);
    //bif->uptime = dev_sys_uptime();
    snprintf(bif->sw_version, sizeof(bif->sw_version), "%s", "v123456");
    snprintf(bif->hw_version, sizeof(bif->hw_version), "%s", "hw");
    return 0;
}



