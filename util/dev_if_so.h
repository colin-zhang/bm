#ifndef _DEV_IF_SO_H
#define _DEV_IF_SO_H 

int dev_get_port(unsigned short port);
int dev_get_addr_by_id(int slot_id, struct sockaddr_in *addr);
int dev_socket_set_broad_cast(int socket_fd, bool bcst);
int dev_sockst_set_reuse(int socket_fd, bool rs);
int dev_afinet_pton(char *ip, struct sockaddr_in *addr);
const char* dev_afinet_ntop(char *ip, int ip_len, struct sockaddr_in *addr);
int dev_get_io_buff_len(int fd, int *buff_len);
int dev_set_nonblocking(int fd, bool if_noblock);

#endif
