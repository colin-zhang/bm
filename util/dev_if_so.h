#ifndef _DEV_IF_SO_H
#define _DEV_IF_SO_H 
#include <netinet/in.h>
#include <stdint.h>
#include <stdbool.h>

int dev_get_port(unsigned short port);

int dev_socket_set_broad_cast(int socket_fd, bool bcst);
int dev_sockst_set_reuse(int socket_fd, bool rs);
int dev_set_nonblocking(int fd, bool if_noblock);

int dev_afinet_pton(char *ip, struct sockaddr_in *addr);
const char* dev_afinet_ntop(char *ip, int ip_len, struct sockaddr_in *addr);

int dev_get_io_buff_len(int fd, int *buff_len);


int dev_get_addr_by_id(int slot_id, struct sockaddr_in *addr);
int dev_get_id_by_addr(struct sockaddr_in *addr, int *slot_id);

#endif
