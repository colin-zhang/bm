#include "dev_event.h"
#include "stdbool.h"
#include <stdio.h>
#include <stdlib.h>

struct _dev_event_t 
{
    int fd;
    dev_event_type_t type;
    dev_epoll_type_t ep_type;
    bool handing;
    handler_t handler;
    destroy_t destroy;
    struct _dev_event_t *next;
    void *data;
    void *handle_ptr;
    char priv[0];
};

dev_event_t* 
dev_event_creat(int fd, dev_event_type_t type, dev_epoll_type_t ep_type, int priv_len)
{
    dev_event_t *ev_prt = NULL;
    ev_prt = calloc(1, sizeof(dev_event_t) + priv_len);
    if (ev_prt == NULL) {
        return NULL;
    }
    ev_prt->fd = fd;
    ev_prt->type = type;
    ev_prt->ep_type = ep_type;
    ev_prt->handle_ptr = ev_prt;
    return ev_prt;
}

void
dev_event_set_data(dev_event_t *event_ptr, void *data, handler_t handler, destroy_t destroy)
{
    event_ptr->data = data;
    event_ptr->handler = handler;
    event_ptr->destroy = destroy;
}

void
dev_event_set_property(dev_event_t *event_ptr, bool handing)
{
    event_ptr->handing = handing;
}

void
dev_event_set_ep_type(dev_event_t *event_ptr, dev_epoll_type_t ep_type)
{
    event_ptr->ep_type = ep_type;
}

dev_event_t* 
dev_event_get_next(dev_event_t *event_ptr)
{
    return event_ptr->next;
}

void*
dev_event_get_priv(dev_event_t *event_ptr)
{
    return event_ptr->priv;
}

void*
dev_event_get_data(dev_event_t *event_ptr)
{
    return event_ptr->data;
}

int 
dev_event_get_fd(dev_event_t *event_ptr)
{
    return event_ptr->fd;
}

dev_event_type_t 
dev_event_get_type(dev_event_t *event_ptr)
{
    return event_ptr->type;
}

dev_epoll_type_t 
dev_event_get_ep_type(dev_event_t *event_ptr)
{
    return event_ptr->ep_type;
}

bool 
dev_event_get_ep_handing(dev_event_t *event_ptr)
{
    return event_ptr->handing;
}

handler_t
dev_event_get_handler(dev_event_t *event_ptr, void **handle_ptr, bool *handing)
{
    if (handle_ptr != NULL) {
        *handle_ptr = event_ptr->handle_ptr;
    }
    if (handing != NULL) {
        *handing = event_ptr->handing;
    }

    return event_ptr->handler;
}

void* 
dev_event_get_handler_ptr(dev_event_t *event_ptr)
{
    return event_ptr->handle_ptr;
}

destroy_t
dev_event_get_destroy(dev_event_t *event_ptr)
{
    return event_ptr->destroy;
}

void
dev_event_set_next(dev_event_t *curr, dev_event_t *next)
{
    curr->next = next;
}

