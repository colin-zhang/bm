#include "./def/dev_def.h"
#include "core/dev_event_loop.h"
#include "core/dev_event.h"
#include "core/dev_event_timer.h"
#include "core/dev_signalfd.h"
#include "util/dev_udp.h"
#include "util/dev_if_so.h"
#include "util/dev_utils.h"

#include "dev_main.h"


int 
board_info_cmp(const void *a, const void *b)
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
ret_boards_adjust(master_info_t *mif, int index)
{
    while (index < mif->reg_board_num) {
        mif->boards[index] = mif->boards[index + 1];
        index++;
    }
    return 0;
}

int
reg_boards_sort(master_info_t *mif)
{
    qsort(mif->boards, mif->reg_board_num, sizeof(board_info_t *), board_info_cmp);
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
    mif->boards[mif->reg_board_num] = bif;
    mif->reg_board_num++;
    reg_boards_sort(mif);
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
        ret_boards_adjust(mif, index);
        mif->reg_board_num--;
        return 0;
    }
    return -1;
}

void 
reg_board_print(master_info_t *mif)
{
    int i;
    for (i = 0; i < mif->reg_board_num; i++) {
        printf("%d\t", mif->boards[i]->slot_id);
    }
    printf("end\n");
}

int 
probe_master_hander(void *ptr, void *ptr_self)
{
    dev_routine_t *rt = (dev_routine_t *)ptr_self;

    dev_sent_msg(rt->ofd, 1, dev_heart_beat('1', '2'));
    fprintf(stdout, "probe_master_hander\n");

    
    return 0;
}

int 
master_io_disp(void *ptr)
{
    return 0;
}

struct master_info *
dev_master_info_init(void *rt)
{
    struct master_info * mif_ptr = NULL;

    mif_ptr = calloc(1, sizeof(struct master_info));

    mif_ptr->boards = calloc(MAX_BOARD_NUM, sizeof(struct board_info));
    mif_ptr->max_board_num = MAX_BOARD_NUM;
    mif_ptr->reg_board_num = 0;
    mif_ptr->rt = rt;

    mif_ptr->probe_timer = dev_sub_timer_creat(0.5, 0, probe_master_hander, rt);
    if (mif_ptr->probe_timer == NULL) {
        exit(-1);
    }
    dev_event_timer_add(mif_ptr->rt->timer, mif_ptr->probe_timer);

    return mif_ptr;
}

dev_event_t *
dev_master_creat(void *data)
{
    dev_event_t *ev_ptr;
    dev_timer_ev_t *timer1 = NULL, *timer2 = NULL, *timer3 = NULL;
    dev_routine_t *rt = (dev_routine_t *)data;
    
    rt->ifd = dev_udp_port_creat(rt->slot_id, dev_protocol_port());
    ev_ptr = dev_event_creat(rt->ifd, DEV_EVENT_IO, EPOLLIN , 0);
    if (ev_ptr == NULL) {
        dbg_Print("ev_ptr, dev_event_creat\n");
        return -1;
    }

    rt->ofd = dev_udp_client_creat();
    rt->state = DEV_STATE_BACKUP;
    rt->td = (struct master_info *) dev_master_info_init(rt);
 
    dev_event_set_data(ev_ptr, rt->td, master_io_disp, NULL);

    return ev_ptr;
}


