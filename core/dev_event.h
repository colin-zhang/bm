#ifndef _DEV_EVENT_h
#define _DEV_EVENT_h 

#include <stdbool.h>
#include <sys/epoll.h>

typedef enum 
{
    DEV_EVENT_IO,
    DEV_EVENT_TIMER,
    DEV_EVENT_SIG,
    DEV_EVENT_EVENTFD,
    DEV_EVENT_FSCHG,
} dev_event_type_t;

typedef enum {
    DEV_EPOLLIN    =  EPOLLIN,  /* The associated file is available for read(2) operations */
    DEV_EPOLLOUT   =  EPOLLOUT, /*The associated file is available for write(2) operations.*/
    DEV_EPOLLRDHUP =  EPOLLRDHUP,/*(since Linux 2.6.17)Stream socket peer closed connection, 
                                or shut down writing half of connection.  
                                (This flag is especially useful for writing  simple  code  to  
                                detect peer shutdown when using Edge Triggered monitoring.) */
    DEV_EPOLLPRI   =  EPOLLPRI, /*There is urgent data available for read(2) operations.*/
    DEV_EPOLLERR   =  EPOLLERR,/*Error condition happened on the associated file descriptor. 
                              epoll_wait(2) will always wait for this event;
                               it is not necessary to set it in events.*/
    DEV_EPOLLHUP   =  EPOLLHUP,/*Hang up happened on the associated file descriptor. 
                              epoll_wait(2) will always wait for this event; 
                              it is not necessary to set it in events.*/

    DEV_EPOLLET    =  EPOLLET,/*Sets the Edge Triggered behavior for the associated file descriptor. 
                             The default behavior for epoll is Level Triggered. 
                             See epoll(7) for more detailed information about Edge 
                             and Level Triggered event distribution architectures.*/
       
    DEV_EPOLLONESHOT  =  EPOLLONESHOT,
} dev_epoll_type_t;

typedef int  (*handler_t)(void *);
typedef void (*destroy_t)(void *);

struct _dev_event_t;
typedef struct _dev_event_t dev_event_t;

void* dev_event_get_priv(dev_event_t *event_ptr);
void* dev_event_get_data(dev_event_t *event_ptr);
int   dev_event_get_fd(dev_event_t *event_ptr);
dev_event_type_t dev_event_get_type(dev_event_t *event_ptr);
dev_epoll_type_t dev_event_get_ep_type(dev_event_t *event_ptr);
dev_event_t* dev_event_get_next(dev_event_t *event_ptr);
bool dev_event_get_ep_handing(dev_event_t *event_ptr);
handler_t dev_event_get_handler(dev_event_t *event_ptr, void **handle_ptr, bool *handing);
void* dev_event_get_handler_ptr(dev_event_t *event_ptr);
destroy_t dev_event_get_destroy(dev_event_t *event_ptr);

dev_event_t* dev_event_creat(int fd, dev_event_type_t type, dev_epoll_type_t ep_type, int priv_len);
void dev_event_set_data(dev_event_t *event_ptr, void *data, handler_t handler, destroy_t destroy);
void dev_event_set_property(dev_event_t *event_ptr, bool handing);
void dev_event_set_ep_type(dev_event_t *event_ptr, dev_epoll_type_t ep_type);
void dev_event_set_next(dev_event_t *curr, dev_event_t *next);

#define DEV_DECL_PRIV(event_ptr, priv) priv_data_t* priv = (priv_data_t*)(dev_event_get_priv(event_ptr))
#define DEV_DECL_FD(event_ptr, fd)   int fd = (int)(dev_event_get_fd(event_ptr))
#define DEV_DECL_DATA(event_ptr, type, data)   (type *) data = (type *)(dev_event_get_data(event_ptr))

#endif
