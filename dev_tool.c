#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <arpa/inet.h>
#include "dev_lib.h"

#include "util/dev_udp.h"
#include "util/dev_if_so.h"

static char g_buf[1024] = {0};
static uint16_t portNum = 8889;

static inline void
api_msg_head(dev_api_msg_head_t *msg, uint8_t type, uint16_t len)
{
    msg->ver = 0x01;
    msg->type = type;
    if (len) msg->date_len = htons(len);
}

static int 
api_reboot(void)
{
    dev_api_msg_head_t *msg = (dev_api_msg_head_t *)g_buf;
    api_msg_head(msg, DEV_CMD_REBOOT, 0);
    return (sizeof(dev_api_msg_head_t));
}

void tool_reboot(int slotid)
{
    int fd = dev_udp_client_creat();
    int msg_len = api_reboot();
    dev_udp_send_to_id(fd, slotid, portNum, g_buf, msg_len);
}

int main(int argc, char *argv[])
{
    int opt_index = 0, opt = 0;
    int loop = 0;
    int slotid;

    /* optional_argument */
    struct option long_opts[] = {
        {"help", no_argument, 0, 'h'},
        {"reboot", required_argument, 0, 2},
    };

    while ((opt = getopt_long(argc, argv, "", long_opts, &opt_index)) != -1) 
    {
        switch (opt) 
        {
            case 2:
                slotid = atoi(optarg);
                tool_reboot(slotid);
                exit(0);
                break;
            default: 
                exit(0);
        }
    }

    return 0;
}