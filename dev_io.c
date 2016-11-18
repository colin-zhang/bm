#include "./def/dev_def.h"
#include "core/dev_event_loop.h"
#include "core/dev_event.h"
#include "core/dev_signalfd.h"
#include "util/dev_udp.h"
#include "util/dev_if_so.h"

#include "dev_protocol.h"
#include "dev_board.h"
#include "dev_master.h"
#include "dev_board_api.h"
#include "dev_io.h"

typedef struct io_info
{
    dev_routine_t *rt;
    dev_timer_ev_t *check_timer;
    dev_timer_ev_t *master_timeout;
    int master_slot;
    int rev_buff_len;
    char *rev_buff;
    long state_conter;
}io_info_t;

static int 
io_checker(void *ptr, void *ptr_self)
{
    io_info_t *ioif = (io_info_t *)ptr_self;
    dev_routine_t *rt = (dev_routine_t *)ioif->rt;
    board_info_t *self_bif = (board_info_t *)(ioif->rt->self_info);

    return 0;
}

static int 
dev_io_master_timeout(void *ptr, void *ptr_self)
{
    io_info_t *ioif = (io_info_t *)ptr_self;
    dev_routine_t *rt = (dev_routine_t *)ioif->rt;
    board_info_t *self_bif = (board_info_t *)(ioif->rt->self_info);
    
    if (self_bif->slot_type != DEV_STATE_IO) {
        self_bif->slot_type = DEV_STATE_IO;
    }
    dev_sub_timer_modify_timeout(ioif->master_timeout, (double)UINT32_MAX);
    return 0;
}

static int 
io_disp_probe(io_info_t *ioif, char *msg, int slotid)
{
    dev_routine_t *rt = (dev_routine_t *)ioif->rt;
    msg_head_t *msg_head = (msg_head_t *)msg;
    msg_probe_t *probe = (msg_probe_t *)msg_head->data;
    board_info_t *self_bif = (board_info_t *)(ioif->rt->self_info);

    if (msg_head->slot_type == DEV_STATE_MASTER ) {
        switch (self_bif->slot_type) {
            case DEV_STATE_IO:
                dev_sent_msg(rt->ofd, slotid, dev_io_register(1));
                break;
            case DEV_STATE_IO_REG: 
                if (slotid == ioif->master_slot) {
                    ioif->state_conter++;
                    dev_sub_timer_modify_timeout(ioif->master_timeout, 2.0);
                }
                dev_sent_msg(rt->ofd, slotid, dev_heart_beat(1)); 
                break;
            default:
                dev_sent_msg(rt->ofd, slotid, dev_heart_beat(1));
                break;
        }
    }
    return 0;
}

static int 
io_disp_register_ack(io_info_t *ioif, char *msg, int slotid)
{
    dev_routine_t *rt = (dev_routine_t *)ioif->rt;
    msg_head_t *msg_head = (msg_head_t *)msg;
    msg_probe_t *probe = (msg_probe_t *)msg_head->data;
    board_info_t *self_bif = (board_info_t *)(ioif->rt->self_info);

    if (msg_head->slot_type == DEV_STATE_MASTER) {
        if (self_bif->slot_type == DEV_STATE_IO) {
            ioif->master_slot = slotid;
            self_bif->slot_type = DEV_REGISTER;
            printf("regestter\n");
        }
    }
    return 0;
}

static int 
io_io_disp(void *ptr)
{
    DEV_DECL_FD(ptr, fd);
    io_info_t *ioif = (io_info_t *)dev_event_get_data(ptr);   
    int rsv_len = ioif->rev_buff_len;
    msg_head_t *msg_head = (msg_head_t *)ioif->rev_buff;
    int rsv_slot = 0;

    memset(ioif->rev_buff, 0, ioif->rev_buff_len);
    dev_udp_rev_from_id(fd, ioif->rev_buff, ioif->rev_buff_len, &rsv_slot);

    switch (msg_head->type) {
        case DEV_RPROBE:
            io_disp_probe(ioif, ioif->rev_buff, rsv_slot);
            break;
        case DEV_REGISTER_ACK:
            io_disp_register_ack(ioif, ioif->rev_buff, rsv_slot);
            break;
        default:
            break;
    }
    return 0;
}


dev_event_t *
dev_io_creat(void *data)
{
    dev_event_t *ev_ptr = NULL;
    dev_routine_t *rt = (dev_routine_t *)data;
    io_info_t *ioif;
        
    ev_ptr = dev_event_creat(rt->ifd, DEV_EVENT_IO, EPOLLIN | DEV_EPOLLET, 0);
    if (ev_ptr == NULL) {
        dbg_Print("ev_ptr, dev_event_creat\n");
        return NULL;
    }

    ioif = calloc(1, sizeof(io_info_t));
    if (ioif == NULL) {
        exit(-1);
    }
    ioif->check_timer = dev_sub_timer_creat(2.0, 0, io_checker, ioif);
    if (ioif->check_timer == NULL) {
        exit(-1);
    }
    ioif->master_timeout = dev_sub_timer_creat((double)UINT32_MAX, 0, dev_io_master_timeout, ioif);
    if (ioif->master_timeout == NULL) {
        exit(-1);
    }

    dev_event_timer_add(rt->timer, ioif->check_timer);
    dev_event_timer_add(rt->timer, ioif->master_timeout);

    ioif->rev_buff_len = 1024;
    ioif->rev_buff = calloc(1, ioif->rev_buff_len);
    if (ioif->rev_buff == NULL) {
        exit(-1);
    }

    rt->td = ioif;
    ioif->rt = rt;
    
    dev_event_set_data(ev_ptr, (io_info_t *)ioif, io_io_disp, NULL);

    return ev_ptr;
}
