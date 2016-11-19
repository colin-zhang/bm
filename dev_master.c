#include "./def/dev_def.h"
#include "core/dev_event_loop.h"
#include "core/dev_event.h"
#include "core/dev_event_timer.h"
#include "core/dev_signalfd.h"
#include "util/dev_udp.h"
#include "util/dev_if_so.h"
#include "util/dev_utils.h"
#include "util/dev_log.h"
#include "dev_protocol.h"
#include "dev_board.h"
#include "dev_master.h"
#include "dev_board_api.h"

int 
reg_board_info_cmp_slot_id(const void *a, const void *b)
{
    /* ascending order */
    board_info_t ** aa = (board_info_t **)a;
    board_info_t ** bb = (board_info_t **)b;

    if ((*aa)->slot_id > (*bb)->slot_id) {
        return 1;
    } else if((*aa)->slot_id == (*bb)->slot_id) {
        return 0;
    } else {
        return -1;
    }
}

int 
reg_boards_adjust(master_info_t *mif, int index)
{
    while (index < mif->reg_board_num) {
        mif->boards[index] = mif->boards[index + 1];
        index++;
    }
    return 0;
}

int
reg_boards_sort_by_slotid(master_info_t *mif)
{
    qsort(mif->boards, mif->reg_board_num, sizeof(board_info_t *), reg_board_info_cmp_slot_id);
    return 0;
}

int 
reg_boards_bsearch(master_info_t *mif, int slot_id) 
{
    int left = 0;
    int right = mif->reg_board_num - 1;
    while (left <= right) {
        int mid = left + ((right - left) >> 1);
        if (mif->boards[mid]->slot_id == slot_id) {
            return mid;
        }
        else if (mif->boards[mid]->slot_id > slot_id) {
            right = mid - 1;
        }
        else {
            left = mid + 1;
        }
    }
    return -1;
}

board_info_t * 
reg_boards_search(master_info_t *mif, int slot_id)
{
    int index = 0;

    index = reg_boards_bsearch(mif, slot_id);
    if (index < 0) {
        return NULL;
    }

    return mif->boards[index];
}

int
reg_boards_add(master_info_t *mif, board_info_t *bif)
{
    int index = reg_boards_bsearch(mif, bif->slot_id);
    if (index < 0) {
        mif->boards[mif->reg_board_num] = bif;
        mif->reg_board_num++;
        reg_boards_sort_by_slotid(mif);
    }
    return 0;
}

int
reg_boards_update(master_info_t *mif, board_info_t *bif)
{
    int index = reg_boards_bsearch(mif, bif->slot_id);\
    if (index < 0) {
        return -1;
    }
    mif->boards[mif->reg_board_num] = bif;
    mif->reg_board_num++;
    reg_boards_sort_by_slotid(mif);
    return 0;
}

int 
reg_boards_del(master_info_t *mif, int slot_id)
{
    board_info_t *t = NULL;
    int index = reg_boards_bsearch(mif, slot_id);
    
    if (index < 0) {
        return -1;
    }

    t = mif->boards[index];
    if (t != NULL) {
        free(t);
        mif->boards[index] = NULL;
        reg_boards_adjust(mif, index);
        mif->reg_board_num--;
        return 0;
    }
    return -1;
}

int
reg_boards_check(master_info_t *mif)
{
    int i = 0;
    for (i = 0; i < mif->reg_board_num; i++) {
        if (mif->boards[i]->timeout_chk) {
            mif->boards[i]->slot_type = DEV_SATE_IO_OFFLINE;
            return 1;
        } else {
            mif->boards[i]->timeout_chk = 1;
        }
    }
    return 0;
}

void 
reg_board_print(master_info_t *mif)
{
    int i;

    printf("register board number = %d\n", mif->reg_board_num);

    for (i = 0; i < mif->reg_board_num; i++) {
        printf(
            "slotid=%d"
            ", slotype=%d"
            ", uptime=%ld"
            "  \n", 
            mif->boards[i]->slot_id,
            mif->boards[i]->slot_type,
            mif->boards[i]->uptime
            );
    }
}

static int 
probe_master_hander(void *ptr, void *ptr_self)
{
    master_info_t *mif = (master_info_t *)ptr_self;
    dev_routine_t *rt = (dev_routine_t *)mif->rt;
    board_info_t *self_bif = (board_info_t *)(mif->rt->self_info);
    int i = 0;
    int master_slot = 0;

/*    if (self_bif->slot_type  == DEV_STATE_BACKUP) {
        master_slot = dev_master_group_chief_slotid(rt->master_group);
        dev_sent_msg(mif->rt->ofd, master_slot, dev_master_probe(1, 0));
    } else {*/
    for (i = 1; i <= 14; i++) {
         if (i != self_bif->slot_id) {
            dev_sent_msg(mif->rt->ofd, i, dev_master_probe(1, 0));
         }
    }

    rt->self_info->uptime = dev_sys_uptime();
    return 0;
}

static int
master_checker(void *ptr, void *ptr_self)
{
    master_info_t *mif = (master_info_t *)ptr_self;
    dev_routine_t *rt = (dev_routine_t *)mif->rt;
    board_info_t *self_bif = (board_info_t *)(mif->rt->self_info);

    if (dev_master_group_probe_timeout_check(rt->master_group, 1)) {
        dev_master_group_select_chief(rt->master_group);
    }

    if (self_bif->slot_type == DEV_STATE_MASTER) {
        reg_boards_check(mif);
    }
    
    dev_master_group_print(rt->master_group);
    reg_board_print(mif);

    return 0;
}

static int 
master_elect(void *ptr, void *ptr_self)
{

    master_info_t *mif = (master_info_t *)ptr_self;
    dev_routine_t *rt = (dev_routine_t *)mif->rt;
    board_info_t *self_bif = (board_info_t *)(mif->rt->self_info);

    printf("self_bif->slot_type = %d \n", self_bif->slot_type);
    dev_master_group_select_chief(rt->master_group);

    printf("chief_index = %d, slotid = %d \n", rt->master_group->chief_index, dev_master_group_chief_slotid(rt->master_group));
    printf("self_bif->slot_type = %d \n", self_bif->slot_type);
    printf("master->slot_type = %d \n", rt->master_group->member[rt->master_group->chief_index]->slot_type);
    fflush(stdout);
    
    return 0;
}

static char rsv_data[2048] = {0};

static int 
master_disp_regester(master_info_t *mif, char *msg, int slotid)
{
    dev_routine_t *rt = (dev_routine_t *)mif->rt;
    msg_head_t *msg_head = (msg_head_t *)msg;
    msg_register_t *reg = (msg_register_t *)msg_head->data;
    static int conter = 0;

    board_info_t bif_tmp;
    board_info_t *bif_ptr = NULL;

    bif_tmp.slot_id = msg_head->slot_id;
    bif_tmp.slot_type = msg_head->slot_type;
    bif_tmp.board_type = ntohs(msg_head->board_type);
    bif_tmp.uptime = ntohll(reg->uptime);
    snprintf((char *)bif_tmp.hw_version, sizeof(bif_tmp.hw_version), "%s", reg->hwVersion);
    snprintf((char *)bif_tmp.sw_version, sizeof(bif_tmp.sw_version), "%s", reg->swVersion);
    bif_tmp.timeout_chk = 0;
    

    bif_ptr = reg_boards_search(mif, bif_tmp.slot_id);
    if (bif_ptr != NULL) {
        dev_board_info_update_state(bif_ptr, &bif_tmp);
    } else {
        bif_ptr = dev_board_info_new();
        memcpy(bif_ptr, &bif_tmp, sizeof(bif_tmp));
        reg_boards_add(mif, bif_ptr);
    }

    reg_board_print(mif);

    dev_sent_msg(rt->ofd, slotid, dev_register_ack(1));
    return 0;
}

static int 
master_disp_heartbeat(master_info_t *mif, char *msg, int slotid)
{
    dev_routine_t *rt = (dev_routine_t *)mif->rt;
    msg_head_t *msg_head = (msg_head_t *)msg;
    msg_heartbeat_t *heartbeat = (msg_heartbeat_t *)msg_head->data;

    board_info_t bif_tmp;
    board_info_t *bif_ptr = NULL;

    bif_tmp.slot_id = msg_head->slot_id;

    bif_ptr = reg_boards_search(mif, bif_tmp.slot_id);
    if (bif_ptr != NULL) {
        bif_tmp.slot_type = msg_head->slot_type;
        bif_tmp.uptime = ntohll(heartbeat->uptime);
        bif_tmp.timeout_chk = 0;
        dev_board_info_update_state(bif_ptr, &bif_tmp);
    }
    return 0;
}

static int 
master_disp_probe(master_info_t *mif, char *msg, int slotid)
{
    dev_routine_t *rt = (dev_routine_t *)mif->rt;
    msg_head_t *msg_head = (msg_head_t *)msg;
    msg_probe_t *probe = (msg_probe_t *)msg_head->data;
    static int conter = 0;

    dev_sent_msg(rt->ofd, slotid, dev_master_probe_ack(1));
    return 0;
}

static int
master_disp_probe_ack(master_info_t *mif, char *msg)
{
    dev_routine_t *rt = (dev_routine_t *)mif->rt;
    msg_head_t *msg_head = (msg_head_t *)msg;
    msg_probe_ack_t *probe_ack = (msg_probe_ack_t *)msg_head->data;
    board_info_t *self_bif = (board_info_t *)(rt->self_info);

    board_info_t bif_tmp;
    int ret = 0;

    bif_tmp.slot_id = msg_head->slot_id;
    bif_tmp.slot_type = msg_head->slot_type;
    bif_tmp.board_type = ntohs(msg_head->board_type);
    bif_tmp.uptime = ntohll(probe_ack->uptime);
    snprintf((char *)bif_tmp.hw_version, sizeof(bif_tmp.hw_version), "%s", probe_ack->hwVersion);
    snprintf((char *)bif_tmp.sw_version, sizeof(bif_tmp.sw_version), "%s", probe_ack->swVersion);
    bif_tmp.timeout_chk = 0;
    
    ret = dev_master_group_add(rt->master_group, &bif_tmp);
    if (bif_tmp.slot_type == DEV_STATE_TOBE_MASTER) {
        dev_master_group_select_chief(rt->master_group);
    }

    if (self_bif->slot_type == DEV_STATE_MASTER && bif_tmp.slot_type == DEV_STATE_MASTER) {
        dev_master_group_select_chief(rt->master_group);
    }

    return 0;
}

static int 
master_io_disp(void *ptr)
{
    DEV_DECL_FD(ptr, fd);
    master_info_t *mif = (master_info_t *)dev_event_get_data(ptr);   
    int rsv_len = sizeof(rsv_data);
    int rsv_slot = 0;
    msg_head_t *msg_head = (msg_head_t *)rsv_data;

    memset(rsv_data, 0, sizeof(rsv_data));
    dev_udp_rev_from_id(fd, rsv_data, rsv_len, &rsv_slot);

    switch (msg_head->type) {
        case DEV_RPROBE:
            master_disp_probe(mif, rsv_data, rsv_slot);
            break;
        case DEV_RPROBE_ACK:
            master_disp_probe_ack(mif, rsv_data);
            break;
        case DEV_REGISTER:
            master_disp_regester(mif, rsv_data, rsv_slot);
            break;
        case DEV_HEARTBEAT:
            master_disp_heartbeat(mif, rsv_data, rsv_slot);
        default:
            break;
    }
    
    return 0;
}

static master_info_t *
dev_master_info_init(void *rt)
{
    master_info_t * mif_ptr = NULL;

    mif_ptr = calloc(1, sizeof(master_info_t));

    mif_ptr->boards = calloc(MAX_BOARD_NUM, sizeof(board_info_t));
    mif_ptr->max_board_num = MAX_BOARD_NUM;
    mif_ptr->reg_board_num = 0;
    mif_ptr->rt = (dev_routine_t *)rt;

    mif_ptr->probe_timer = dev_sub_timer_creat(0.5, 0, probe_master_hander, mif_ptr);
    if (mif_ptr->probe_timer == NULL) {
        exit(-1);
    }
    mif_ptr->check_timer = dev_sub_timer_creat(2.0, 0, master_checker, mif_ptr);
    if (mif_ptr->check_timer == NULL) {
        exit(-1);
    }
    mif_ptr->vote_timer = dev_sub_timer_creat(1.5, 1, master_elect, mif_ptr);
    if (mif_ptr->vote_timer == NULL) {
        exit(-1);
    }

    dev_event_timer_add(mif_ptr->rt->timer, mif_ptr->probe_timer);
    dev_event_timer_add(mif_ptr->rt->timer, mif_ptr->check_timer);
    dev_event_timer_add(mif_ptr->rt->timer, mif_ptr->vote_timer);
    return mif_ptr;
}

dev_event_t *
dev_master_creat(void *data)
{
    dev_event_t *ev_ptr;
    dev_routine_t *rt = (dev_routine_t *)data;
    master_info_t *mif;
        
    ev_ptr = dev_event_creat(rt->ifd, DEV_EVENT_IO, EPOLLIN/*| DEV_EPOLLET */, 0);
    if (ev_ptr == NULL) {
        dbg_Print("ev_ptr, dev_event_creat\n");
        return NULL;
    }

    mif = dev_master_info_init(rt);
    rt->td = mif;

    dev_event_set_data(ev_ptr, mif, master_io_disp, NULL);

    return ev_ptr;
}
