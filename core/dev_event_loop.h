#ifndef _DEV_EVENT_LOOP_H
#define _DEV_EVENT_LOOP_H

#include "dev_event.h"
#include <sys/epoll.h>

typedef struct _dev_event_list 
{
    int event_cnt;
    int event_max;
    dev_event_t *head;
    dev_event_t *tail;
} dev_event_list_t;


typedef struct _dev_event_loop
{
    int ep_fd;
    int ev_max;
    struct epoll_event *ep_events;
    dev_event_list_t *event_list;
} dev_event_loop_t;

dev_event_loop_t * dev_event_loop_creat(int max_event);
int dev_event_loop_run(dev_event_loop_t* loop);
int dev_event_loop_add(dev_event_loop_t* loop, dev_event_t *event_ptr);
int dev_event_loop_remove(dev_event_loop_t* loop, dev_event_t *event_ptr);
dev_event_loop_t * dev_event_deafult_loop_init(int max_event);
dev_event_loop_t * dev_event_deafult_loop(void);

#endif