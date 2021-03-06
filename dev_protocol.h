#ifndef _DEV_PROTOCOL_H
#define _DEV_PROTOCOL_H

#include <stdint.h>

#pragma pack(2)

//globle packet head
typedef struct _msg_head
{
   uint8_t   version;
   uint8_t   type;
   uint8_t   slot_id;
   uint8_t   slot_type;
   uint32_t  board_type;
   uint16_t  len;
   int8_t    data[0];
}msg_head_t;

enum protocol_type
{
    DEV_RPROBE = 1,
    DEV_RPROBE_ACK,
    DEV_HEARTBEAT,
    DEV_REGISTER,
    DEV_REGISTER_ACK,
    DEV_CMD_MAX
};

//主控板探测报文
typedef struct _msg_probe
{
    uint32_t  seq;
    uint64_t  uptime;
    uint32_t  flag;
}msg_probe_t;


typedef struct _msg_probe_ack
{
    uint32_t  seq;
    uint64_t  uptime;
    uint8_t   hwVersion[32];
    uint8_t   swVersion[32];
}msg_probe_ack_t;


//业务板注册报文
typedef struct _msg_register
{
    uint32_t  seq;
    uint64_t  uptime;
    uint8_t   hwVersion[32];
    uint8_t   swVersion[32];
}msg_register_t;

//注册确认报文/业务板心跳/去注册
typedef struct _msg_register_ack
{
    uint32_t  seq;
    uint32_t  sessionID;
}msg_register_ack_t;


typedef struct _msg_heartbeat
{
    uint32_t  seq;
    uint64_t  uptime;
}msg_heartbeat_t;

#pragma pack()

int dev_protocol_port(void);

int dev_master_probe(int seq, int flag);
int dev_master_probe_ack(int seq);

int dev_io_register(int seq);
int dev_register_ack(int seq);

int dev_heart_beat(int seq);

int dev_set_master(void);

int dev_sent_msg(int fd, int slotid, int msg_len);


#endif