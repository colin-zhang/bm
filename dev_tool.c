#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <poll.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include "dev_lib.h"

#include "util/dev_udp.h"
#include "util/dev_if_so.h"

static uint16_t portNum = 8889;

static int 
nonblocking(int fd) 
{
    int flags;
    if (-1 == (flags = fcntl(fd, F_GETFL, 0))) {
        perror("setnonblocking error");
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int 
io_buff_len(int fd)
{
    int len = 0;
    ioctl(fd, FIONREAD, &len);
    return len;
}

static int 
io_poll(int fd, int timeout_ms) 
{
    struct pollfd fds[1];
    int ret;
    fds[0].fd = fd;
    fds[0].events = POLLIN | POLLPRI;
    if ((ret = poll(fds, 1, timeout_ms)) < 0)
        return -1;
    if (ret)
        return 1;
    // time out 
    return 0;
}

static inline void
api_msg_head(dev_api_msg_head_t *msg, uint8_t type, uint16_t len)
{
    msg->ver = 0x01;
    msg->cmd = type;
    if (len) msg->data_len = htons(len);
}

static int 
api_cmd_sent(int slotid, uint8_t cmd, int *fd)
{
    static char g_buf[1024] = {0};
    *fd = dev_udp_client_creat_con(slotid, portNum);
    if (*fd < 0) {
        return -1;
    }
    dev_api_msg_head_t *msg = (dev_api_msg_head_t *)g_buf;
    api_msg_head(msg, cmd, 0);
    return dev_udp_send_con(*fd, g_buf, sizeof(dev_api_msg_head_t));
}

static int 
api_cmd_receive(int fd, dev_api_msg_t *msg)
{
    int ret = io_poll(fd, 1000);
    int res = 0;
    int len = 0;
    if (ret > 0) {
        len = io_buff_len(fd);
        printf("io buff len = %d\n", len);
        res = dev_udp_receive_con(fd, (char *)msg, sizeof(dev_api_msg_t));
        if (res < 0) { return -1; }
    } else if (ret == 0) {
        fprintf(stderr, "%s\n", "time out");
        return -1;
    } else {
        fprintf(stderr, "%s\n", "poll error");
        return -1;
    }

    if (res > 0) {
/*        if (msg->header.code != 0x30) {
            fprintf(stderr, "error, %s\n", msg->error_msg);
            return -1;
        }*/
    } 
    return 0;
}

static int 
api_reboot(int slotid) 
{
    int fd;
    api_cmd_sent(slotid, DEV_CMD_REBOOT, &fd);
    close(fd);
    return 0;
}

static int 
api_boardinfo(int slotid)
{   
    int fd, ret;
    dev_api_board_info_t b_info;
    dev_api_msg_t msg;

    ret = api_cmd_sent(slotid, DEV_CMD_GET_BOARD_INFO, &fd);
    if (ret < 0) {
        close(fd);
        return -1;
    }
    ret = api_cmd_receive(fd, &msg);
    if (ret < 0) {
        close(fd);
        return -1;
    }
    close(fd);

    b_info.slot_id = msg.board_info.slot_id;
    b_info.slot_type = msg.board_info.slot_type;
    b_info.board_type = ntohs(msg.board_info.board_type);
    b_info.uptime = ntohl(msg.board_info.uptime);
    snprintf((char *)b_info.hw_version, sizeof(b_info.hw_version), "%s", msg.board_info.hw_version);
    snprintf((char *)b_info.sw_version, sizeof(b_info.hw_version), "%s", msg.board_info.sw_version);

    printf("slot_id=%d, slot_type=%d, board_type=%08x, uptime=%d, hVer=%s, sVer=%s\n", 
            b_info.slot_id, 
            b_info.slot_type,
            b_info.board_type, 
            b_info.uptime,
            b_info.hw_version,
            b_info.sw_version);

    return 0;
}

static int 
api_test(int slotid) 
{
    int fd, ret;
    dev_api_msg_t msg;

    ret = api_cmd_sent(slotid, DEV_CMD_GET_BOARD_INFO, &fd);
    if (ret < 0) {
        close(fd);
        return -1;
    }
    ret = api_cmd_receive(fd, &msg);
    if (ret < 0) {
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
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
        {"info", required_argument, 0, 3},
        {"test", required_argument, 0, 4},
    };

    while ((opt = getopt_long(argc, argv, "", long_opts, &opt_index)) != -1) 
    {
        switch (opt) 
        {
            case 2:
                slotid = atoi(optarg);
                api_reboot(slotid);
                exit(0);
                break;
            case 3:
                slotid = atoi(optarg);
                api_boardinfo(slotid);
                exit(0);
            case 4:
                slotid = atoi(optarg);
                api_test(slotid);
                exit(0);
            default: 
                exit(0);
        }
    }

    return 0;
}
