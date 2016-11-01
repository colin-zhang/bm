#ifndef _DEV_PROTOCOL_H
#define _DEV_PROTOCOL_H

#include <stdint.h>

static inline uint64_t htonll(uint64_t val) { 
     return  (((uint64_t) htonl(val))  <<   32 )  +  htonl(val  >>   32 ); 
} 

static inline uint64_t ntohll(uint64_t val) { 
     return  (((uint64_t) ntohl(val))  <<   32 )  +  ntohl(val  >>   32 ); 
} 

//globle packet head
typedef struct _msg_head
{
   uint8_t   version;
   uint8_t   type;
   uint16_t  len;
   int8_t    data[0];
}msg_head_t;

enum protocol_type
{
    DEV_RPROBE = 1,
    DEV_RPROBE_ACK,
    DEV_HEARTBEAT,
    DEV_REGISTER,
    DEV_REBOOT,
    DEV_CMD_MAX
};

//主控板探测报文
typedef struct _msg_probe
{
    uint8_t   slot;
    uint8_t   slot_type;
    uint16_t  board_type;
    uint32_t  seq;
    uint64_t  uptime;
    uint32_t  flag;
}msg_probe_t;

//业务板注册报文
typedef struct _msg_register
{
    uint32_t  seq;
    uint32_t  time;
    uint32_t  boardState;
    uint8_t   slot;
    uint64_t  uptime;
    uint8_t   reserved1;
    uint16_t  reserved2;
    uint32_t  boardType;
    uint8_t   hwVersion[32];
    uint8_t   swVersion[32];
}msg_register_t;

//注册确认报文/业务板心跳/去注册
typedef struct _msg_register_ack
{
    uint32_t  seq;
    uint32_t  sessionID;
}msg_register_ack_t;






#endif