#ifndef _DEV_PROTOCOL_H
#define _DEV_PROTOCOL_H

#include <stdint.h>


//主控板探测报文
typedef struct _msg_probe
{
    uint8_t   flag;
    uint8_t   slot;
    uint16_t  reserved;
    uint32_t  seq;
    uint32_t  time;
}msg_probe_t;

//业务板注册报文
typedef struct _msg_register
{
    uint32_t  seq;
    uint32_t  time;
    uint32_t  boardState;
    uint8_t   slot;
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


typedef struct tag_STMODULESERVER
{
    uint32_t  uiModuleId;
}STMODULESERVER;


typedef struct tag_STATESRVSEQ
{
    int iSubNum;
    int iSlotMin;
    int iSlotMax;
}STATESRVSEQ;

enum BoardStateNotify
{
    DEV_BOARDSTATE,
    DEV_LINKSTATE,
    DEV_NOTYFY_MAX
};

typedef struct tag_STATESRV
{
    int iType;
    int iSlotId;
    int iLinkState;
}STATESRV;

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
    DEV_GET_LINKSTATE = 0,          //lib
    DEV_GET_BOARDSTATE,
    DEV_REGISTER_SRV,
    DEV_SHOW_VERSION,          //cli
    DEV_SHOW_SLOT_INFO,
    DEV_SHOW_EXECPTION,    
    DEV_REBOOT,
    DEV_RPROBE,                //board management
    DEV_HEARTBEAT,
    DEV_REGISTER,
    DEV_MASTER_CONFLICT,
    DEV_SLAVE_VOTE,
    DEV_MASTER_EXISTS,
    DEV_MODULE_REGISTERSRV,
    DEV_BOARD_STATE_NOTIFY,
    DEV_BOARD_IF_INFO,
    DEV_CMD_MAX
};




#endif