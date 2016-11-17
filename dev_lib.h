#ifndef _DEV_LIB_H
#define _DEV_LIB_H

#include <stdint.h>

typedef struct dev_api_msg_head_
{
    uint8_t   ver;
    uint8_t   cmd;
    uint16_t  data_len;   
    int8_t    data[0]; 
}dev_api_msg_head_t;

enum dev_api_cmd_type
{
    DEV_CMD_REBOOT = 1,
    DEV_CMD_GET_BOARD_INFO,
};

typedef struct dev_api_board_info
{
    int slot_id;
    int slot_type; // board_state
    int board_type;
    long uptime;  
    char hw_version[32];
    char sw_version[32];
}dev_api_board_info_t;


#endif