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

typedef struct _priv_date_t
{
    board_info_t *self_info;
    dev_master_group_t *master_group;
} api_priv_t;

typedef int (*dev_api_disp_t)(void *priv, const int8_t*, uint16_t, int8_t*, uint32_t*);

#define DECL_API_PRIV(event_ptr, priv) api_priv_t* priv = (api_priv_t*)(dev_event_get_priv(event_ptr))

static const uint16_t api_port = 8889;
static dev_api_disp_t dev_api_disp_table[256] = {NULL, };


static inline void
api_msg_head(dev_api_msg_head_t *msg, uint8_t cmd, uint16_t len)
{
    msg->ver = 0x01;
    msg->cmd = cmd;
    msg->code = 0x30;
    if (len > 0) msg->data_len = htons(len);
    else msg->data_len = 0;
}

static inline void
api_msg_head_error(dev_api_msg_head_t *msg, uint8_t cmd, uint8_t code, const char *err_msg)
{
    int len = 0;
    msg->ver = 0x01;
    msg->cmd = cmd;
    msg->code = code;
    len = snprintf((char *)msg->data, 32, "%s", err_msg);
    if (len > 0) msg->data_len = htons(len);
    else msg->data_len = 0;
}

static int 
dev_api_reboot(void *priv, const int8_t *data, uint16_t len, int8_t *ack_buf, uint32_t *ack_len)
{
    system("reboot");
    return 0;
}

static int 
dev_api_get_board_info(void *priv, const int8_t *data, uint16_t len, int8_t *ack_buf, uint32_t *ack_len)
{
    api_priv_t *priv_ptr = (api_priv_t *)priv;
    dev_api_board_info_t *api_bif = (dev_api_board_info_t *)ack_buf;
    *ack_len = sizeof(dev_api_board_info_t);

    printf("dev_api_get_board_info \n");

    api_bif->slot_id = (priv_ptr->self_info->slot_id);
    api_bif->slot_type = (priv_ptr->self_info->slot_type);
    api_bif->board_type = htons(priv_ptr->self_info->board_type);
    api_bif->uptime = htonl(dev_sys_uptime());
    snprintf((char *)api_bif->hw_version, sizeof(api_bif->hw_version), "%s", priv_ptr->self_info->hw_version);
    snprintf((char *)api_bif->sw_version, sizeof(api_bif->sw_version), "%s", priv_ptr->self_info->sw_version);
    return 0;
}

static int 
dev_api_disp(void *api_priv, dev_api_msg_head_t *msg_head, int8_t *ack_buf, uint32_t *ack_len)
{
    int len = 0;
    dev_api_msg_head_t *msg_ack = (dev_api_msg_head_t *)ack_buf;
    if (dev_api_disp_table[msg_head->cmd] == NULL) {
        api_msg_head_error(msg_ack, msg_head->cmd, 0xE1, "not support");
        *ack_len = sizeof(dev_api_msg_t);
        return -1;
    }
    len = dev_api_disp_table[msg_head->cmd](api_priv, msg_head->data, msg_head->data_len, msg_ack->data, ack_len);

    api_msg_head(msg_ack, msg_head->cmd, *ack_len);
    *ack_len = sizeof(dev_api_msg_t);
    return 0;
}

static void
dev_api_table_init()
{
    bzero(dev_api_disp_table, sizeof(dev_api_disp_table));
    dev_api_disp_table[DEV_CMD_REBOOT] = dev_api_reboot;
    dev_api_disp_table[DEV_CMD_GET_BOARD_INFO] = dev_api_get_board_info;
}

static int 
dev_api_io_disp(void *ptr)
{
    char rsv_buff[1024] = {0};
    char ack_buff[4096] = {0};
    DEV_DECL_FD(ptr, sockfd);
    DECL_API_PRIV(ptr, priv_ptr);
    dev_routine_t *rt = (dev_routine_t *)dev_event_get_data(ptr);
    dev_api_msg_head_t *api_msg_head;
    api_msg_head = (dev_api_msg_head_t *)rsv_buff;
    int rsv_len = sizeof(rsv_buff);
    uint32_t ack_len = 0;
    int ret = 0;
    struct sockaddr_in peer_addr;

    printf("dev_api_io_disp\n");

    bzero(rsv_buff, sizeof(rsv_buff));
    dev_udp_receive(sockfd, rsv_buff, rsv_len, &peer_addr);
    dev_api_disp(priv_ptr, api_msg_head, (int8_t *)ack_buff, &ack_len);
    printf("ack_len = %d\n", ack_len);
    fflush(stdout);
    dev_udp_send(sockfd, (char *)ack_buff, ack_len, &peer_addr);
    return 0;
}

dev_event_t *
dev_board_api_init(void *data)
{
    dev_event_t *ev_ptr = NULL;
    dev_routine_t *rt = (dev_routine_t *)data;
    api_priv_t *priv_ptr;

    int fd = dev_udp_port_creat(rt->self_info->slot_id, api_port);
    ev_ptr = dev_event_creat(fd, DEV_EVENT_IO, EPOLLIN | DEV_EPOLLET, sizeof(api_priv_t));
    if (ev_ptr == NULL) {
        dbg_Print("ev_ptr, dev_event_creat\n");
        return NULL;
    }
    priv_ptr = (api_priv_t *)dev_event_get_priv(ev_ptr);
    priv_ptr->self_info = rt->self_info;
    priv_ptr->master_group = rt->master_group;
    dev_event_set_data(ev_ptr, (void *)rt, dev_api_io_disp, NULL);
    dev_api_table_init();
    return ev_ptr;
}

