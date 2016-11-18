#ifndef _DEV_LIB_H
#define _DEV_LIB_H

#include <stdint.h>

/*
code  
    30 OK
    E1 not support
    E2 fail to get
*/

typedef struct dev_api_msg_head_
{
    uint8_t   ver;
    uint8_t   cmd;
    uint16_t  data_len;
    //uint8_t   code;      //return code
    int8_t    data[0];
} dev_api_msg_head_t;

enum dev_api_cmd_type
{
    DEV_CMD_REBOOT = 1,
    DEV_CMD_GET_BOARD_INFO,
};

typedef struct dev_api_board_info
{
    uint8_t slot_id;
    uint8_t slot_type; // board_state
    uint16_t board_type;
    uint32_t uptime;  
    int8_t hw_version[32];
    int8_t sw_version[32];
}dev_api_board_info_t;



typedef struct dev_api_msg
{
    dev_api_msg_head_t  header;
    union {
        dev_api_board_info_t board_info;
        //char error_msg[32];
        
    };
}dev_api_msg_t;


#endif