#include "../def/dev_def.h"

//#ifdef DEV_IPC_FIFO
//O_RDWR|O_NONBLOCK

#define DEV_IPC_FILE_DIR "/run/devd"

int dev_open_fifo(int ipc_chn, int flag)
{
    int fd_fifo, ret, mk_flag;
    char fifo_path[512];

    if( !dev_file_exist(DEV_IPC_FILE_DIR)) {
        dev_make_dir(DEV_IPC_FILE_DIR, 0666);
    }

    snprintf(fifo_path, sizeof(fifo_path), "%s/fifo_%d_.ipc", DEV_IPC_FILE_DIR, ipc_chn);
    if(dev_file_exist(fifo_path)) {
        if (dev_is_fifo(fifo_path)) {
            mk_flag = 0;
        } else {
            dev_delete_file(fifo_path);
            mk_flag = 1;
        }
    } else {
        mk_flag = 1;
    }

    if (mk_flag) {
        ret = mkfifo(fifo_path, 0666);
        if( ret < 0) { 
            return -1;
        }
    }

    fd_fifo = open(fifo_path, flag);
    if(fd_fifo < 0) {  
        return -1;  
    }
    return fd_fifo;
}

int dev_read_fifo(int rd_fd, char *data_buf, int len){

    int ret;
    ret = readn(rd_fd, data_buf, len);
    return ret;
}

int dev_read_fd_time_out(int fd, char *data_buf, int len, int timeout_ms) 
{
    struct pollfd fds[1];
    int ret=0,r=0;

    fds[0].fd = fd;
    fds[0].events = POLLIN | POLLPRI;

    do{
        ret = poll(fds, 1, timeout_ms);
        if(ret > 0){
            break;
        }
    }while(errno == EINTR);
        
    if (ret){
    again:
        r = read(fd, data_buf, len);
        if(errno == EINTR && r == -1)  
        {  
            goto again;  
        }  
        return r;
    }
    // time out 
    return 0;
}

int dev_write_fifo(int wt_fd, void *data_buf,int data_len)
{   
    int ret = 0;
    char *d = (char *)data_buf;
    ret = writen(wt_fd, d, data_len);
    return ret;
}


#include <stdio.h>  
#include <unistd.h>  
#include <sys/un.h>  
#include <sys/socket.h>  
 
int 
dev_unix_socket_bind(int ipc_chn)
{  
    int sockfd;
    struct sockaddr_un addr;
    char path[512];

    snprintf(path, sizeof(path), "%s/unix_%d.ipc", DEV_IPC_FILE_DIR, ipc_chn);

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return -1;
    }

    bzero(&addr, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);

    if (bind(sockfd,(struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return -1;
    }

    return sockfd;
}  

int 
dev_unix_socket_open(int ipc_chn)
{  
    int sockfd;
    struct sockaddr_un addr;
    char path[512];

    snprintf(path, sizeof(path), "%s/unix_%d.ipc", DEV_IPC_FILE_DIR, ipc_chn);

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return -1;
    }

    bzero(&addr, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);

    if (bind(sockfd,(struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return -1;
    }

    return sockfd;
}  

int 
dev_unix_socket_send(int sockfd, int ipc_chn, char *msg, int msg_len)
{  
    int ret;   
    struct sockaddr_un addr;
    char path[512];

    snprintf(path, sizeof(path), "%s/unix_%d.ipc", DEV_IPC_FILE_DIR, ipc_chn);
    bzero(&addr, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);
  
    ret = sendto(sockfd, msg, msg_len, 0, (struct sockaddr*)&addr, sizeof(addr));

    return ret;
} 

int 
dev_unix_socket_receive(int sockfd, char *msg, int len)  
{  
    int n;   

    memset(msg, 0, len);  
    n = recvfrom(sockfd, msg, len, 0, NULL, NULL);   
    if( n <= 0) {  
        return -1;  
    }  
    msg[n] = 0;  

    return n;  
}  

