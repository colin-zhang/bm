#ifndef _DEV_UDP_H
#define _DEV_UDP_H 
#include <arpa/inet.h>

int dev_udp_port_creat(int id, unsigned short port);
int dev_udp_client_creat(void);

int dev_udp_client_creat_con(int server_id, unsigned short server_port);
int dev_udp_send_con(int sockfd, char *msg, int msg_len);
int dev_udp_receive_con(int sockfd, char *rsv, int rsv_len);

int dev_udp_rev_from_id(int sockfd, char *rsv, int rsv_len, int *slot);
int dev_udp_send_to_id(int sockfd, int id, unsigned short port, char *msg, int msg_len);

int dev_udp_send(int sockfd, char *msg, int msg_len, struct sockaddr_in *peer_addr);
int dev_udp_receive(int sockfd, char *rsv, int rsv_len, struct sockaddr_in *peer_addr);

#endif
