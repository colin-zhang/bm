#include "./def/dev_def.h"
#include "core/dev_event_loop.h"
#include "core/dev_event.h"
#include "core/dev_event_timer.h"
#include "core/dev_signalfd.h"
#include "util/dev_udp.h"
#include "util/dev_if_so.h"
#include "util/dev_utils.h"
#include "util/dev_log.h"

#include "dev_board.h"
#include "dev_board_api.h"

static const uint16_t api_port = 8889;

static void dev_reboot()
{
    system("reboot");
}

static int 
dev_api_io_disp(void *ptr)
{
    DEV_DECL_FD(ptr, sockfd);
    dev_routine_t *rt = (dev_routine_t *)dev_event_get_data(ptr);
    dev_api_msg_head_t *api_msg_head;
    char rsv_buff[1024] = {0};
    int rsv_len = sizeof(rsv_buff);
    api_msg_head = (dev_api_msg_head_t *)rsv_buff;
    struct sockaddr_in peer_addr;

    memset(rsv_buff, 0, sizeof(rsv_buff));
    dev_udp_receive(sockfd, rsv_buff, rsv_len, &peer_addr);

    switch (api_msg_head->type) {
        case DEV_CMD_REBOOT:
            dev_reboot();
            break;
        default:
            break;
    }
    return 0;
}

dev_event_t *
dev_board_api_init(void *data)
{
    dev_event_t *ev_ptr = NULL;
    dev_routine_t *rt = (dev_routine_t *)data;

    int fd = dev_udp_port_creat(rt->self_info->slot_id, api_port);
        
    ev_ptr = dev_event_creat(fd, DEV_EVENT_IO, EPOLLIN , 0);
    if (ev_ptr == NULL) {
        dbg_Print("ev_ptr, dev_event_creat\n");
        return NULL;
    }
    dev_event_set_data(ev_ptr, (void *)rt, dev_api_io_disp, NULL);
    return ev_ptr;
}

