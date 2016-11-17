#ifndef _DEV_LIB_H
#define _DEV_LIB_H

#include <stdint.h>

typedef struct dev_api_msg_head_
{
    uint8_t   ver;
    uint8_t   type;
    uint16_t  date_len;   
    int8_t    data[0];    
}dev_api_msg_head_t;

enum dev_api_cmd_type
{
    DEV_CMD_REBOOT = 1,
    DEV_CMD_,
};

#endif