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

typedef int (*dev_api_disp_t)(const int8_t*, uint16_t, int8_t*, uint32_t*);

static const uint16_t api_port = 8889;
static dev_api_disp_t dev_api_disp_table[256] = {NULL, };


static inline void
api_msg_head(dev_api_msg_head_t *msg, uint8_t cmd, uint16_t len)
{
    msg->ver = 0x01;
    msg->cmd = cmd;
    if (len) msg->data_len = htons(len);
}

static int 
dev_reboot(const int8_t *data, uint16_t len, int8_t *ack_buf, uint32_t *ack_len)
{
    system("reboot");
    return 0;
}

static int 
dev_api_disp(dev_api_msg_head_t *msg_head, int8_t *ack_buf, uint32_t *ack_len)
{
    int len = 0;
    dev_api_msg_head_t *msg_ack = (dev_api_msg_head_t *)ack_buf;
    if (dev_api_disp_table[msg_head->cmd] == NULL) {
        return -1;
    }
    len = dev_api_disp_table[msg_head->cmd](msg_head->data, msg_head->data_len, msg_ack->data, ack_len);

    api_msg_head(msg_ack, msg_head->cmd, len);
    return 0;
}

static void
dev_api_table_init()
{
    bzero(dev_api_disp_table, sizeof(dev_api_disp_table));
    dev_api_disp_table[DEV_CMD_REBOOT] = dev_reboot;
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
    int ret;
    struct sockaddr_in peer_addr;

    memset(rsv_buff, 0, sizeof(rsv_buff));
    dev_udp_receive(sockfd, rsv_buff, rsv_len, &peer_addr);
    //dev_api_disp(api_msg_head, );

    //dev_udp_send(sockfd, char *msg, int msg_len, &peer_addr);
    
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

