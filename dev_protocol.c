#include "./def/dev_def.h"
#include "util/dev_udp.h"
#include "util/dev_if_so.h"
#include "util/dev_utils.h"

#include "dev_protocol.h"
#include "dev_board.h"

static char protocol_buf[2048] = {0};
static const int portNum = 8888;
static const int dev_protocol_version = 0x02;

static board_info_t *gp_bif = NULL;

#define MSG_HEAD_LEN sizeof(msg_head_t)

int 
dev_protocol_port(void) 
{
    return portNum;
}

void 
dev_protocol_init_boardinfo(board_info_t *bif)
{
    gp_bif = bif;
}

static inline void
dev_msg_head(msg_head_t *msg, uint8_t type, uint16_t len)
{
    msg->version = dev_protocol_version;
    msg->type = type;
    msg->slot_id = gp_bif->slot_id;
    msg->slot_type = gp_bif->slot_type;
    msg->board_type = htonl(gp_bif->board_type);
    if (len) msg->len = htons(len);
}

/*
    uint8_t   slot;
    uint8_t   slot_type;
    uint16_t  board_type;
    uint32_t  seq;
    uint32_t  uptime;
    uint32_t  flag;
*/
int 
dev_master_probe(int seq, int flag)
{
    msg_head_t *msg = (msg_head_t *)protocol_buf;
    msg_probe_t *probe = (msg_probe_t *)msg->data;
    long uptime = dev_sys_uptime();

    probe->seq = htonl((uint32_t)seq);
    probe->uptime = dev_htonll((uint64_t)uptime);
    probe->flag = htonl((uint32_t)flag);
    dev_msg_head(msg, DEV_RPROBE, sizeof(msg_probe_t));
    return (sizeof(msg_probe_t) + MSG_HEAD_LEN);
}

/*
    uint32_t  seq;
    uint32_t  time;
    uint64_t  uptime;
    uint8_t   hwVersion[32];
    uint8_t   swVersion[32];
*/
int 
dev_master_probe_ack(int seq)
{
    msg_head_t *msg = (msg_head_t *)protocol_buf;
    msg_probe_ack_t *probe_ack = (msg_probe_ack_t *)msg->data;
    long uptime = dev_sys_uptime();

    probe_ack->seq = htonl((uint32_t)seq);
    probe_ack->uptime = dev_htonll((uint64_t)uptime);
    snprintf((char *)probe_ack->hwVersion, sizeof(probe_ack->hwVersion), "%s", gp_bif->hw_version);
    snprintf((char *)probe_ack->swVersion, sizeof(probe_ack->swVersion), "%s", gp_bif->sw_version);

    dev_msg_head(msg, DEV_RPROBE_ACK, sizeof(msg_probe_ack_t));
    return (sizeof(msg_probe_ack_t) + MSG_HEAD_LEN);
}

int 
dev_io_register(int seq)
{
    msg_head_t *msg = (msg_head_t *)protocol_buf;
    msg_register_t *reg = (msg_register_t *)msg->data;
    long uptime = dev_sys_uptime();

    reg->seq = htonl((uint32_t)seq);
    reg->uptime = dev_htonll((uint64_t)uptime);
    snprintf((char *)reg->hwVersion, sizeof(reg->hwVersion), "%s", gp_bif->hw_version);
    snprintf((char *)reg->swVersion, sizeof(reg->swVersion), "%s", gp_bif->sw_version);

    dev_msg_head(msg, DEV_REGISTER, sizeof(msg_register_t));
    return (sizeof(msg_register_t) + MSG_HEAD_LEN);
}

int 
dev_register_ack(int seq)
{
    msg_head_t *msg = (msg_head_t *)protocol_buf;
    msg_register_ack_t *reg_ack = (msg_register_ack_t *)msg->data;

    reg_ack->seq = htonl((uint32_t)seq);
    reg_ack->sessionID = dev_htonll(1);

    dev_msg_head(msg, DEV_REGISTER_ACK, sizeof(msg_register_ack_t));
    return (sizeof(msg_register_t) + MSG_HEAD_LEN);
}

int 
dev_heart_beat(int seq)
{
    msg_head_t *msg = (msg_head_t *)protocol_buf;
    msg_heartbeat_t *hearbeat = (msg_heartbeat_t *)msg->data;
    long uptime = dev_sys_uptime();

    hearbeat->seq = htonl((uint32_t)seq);
    hearbeat->uptime = dev_htonll((uint64_t)uptime);
    
    dev_msg_head(msg, DEV_HEARTBEAT, sizeof(msg_heartbeat_t));
    return (sizeof(msg_heartbeat_t) + MSG_HEAD_LEN);
}

/*
int
dev_set_master(void)
{
    msg_head_t *msg = (msg_head_t *)protocol_buf;
    dev_msg_head(msg, DEV_SET_MASTER, 0);
    return (MSG_HEAD_LEN);
}
*/

int
dev_sent_msg(int fd, int slotid, int msg_len)
{
    int i = 0;
    dev_udp_send_to_id(fd, slotid, portNum, protocol_buf, msg_len);
    return 0;
}

/*
int
dev_sent_msg_array(int fd, slotid_array_t sid_array, int len)
{
    int i = 0;

    while (sid_array[i]) {
       dev_udp_send_to_id(fd, sid_array[i], portNum, protocol_buf, len);
       i++;
    }

    return 0;
}

int
dev_reply_msg(int fd, slotid_array_t sid_array, int len)
{
    int i = 0;

    while (sid_array[i]) {
       dev_udp_send_to_id(fd, sid_array[i], portNum, protocol_buf, len);
       i++;
    }

    return 0;
}


*/