#include "dev_event.h"

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
