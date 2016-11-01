#include "../def/dev_def.h"
#include "dev_event.h"
#include <sys/signalfd.h>  
#include <signal.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>

static int 
dev_signalfd_handler(void *ptr)
{
    struct signalfd_siginfo fdsi;
    int s;
    DEV_DECL_FD(ptr, fd);
    
    s = read(fd, &fdsi, sizeof(struct signalfd_siginfo));
    if (s != sizeof(struct signalfd_siginfo)) {
        return -1;
    }       

    if (fdsi.ssi_signo == SIGINT) {  
        printf("SIGINT\n"); 
    } else if (fdsi.ssi_signo == SIGPIPE) {
        printf("SIGPIPE\n");
    } 
    else if (fdsi.ssi_signo == SIGQUIT) {  
        printf("SIGQUIT\n");  
        exit(EXIT_SUCCESS);
    } else {  
        dbg_Print("unexpected signal\n");  
    }
    
    return 0;
}

dev_event_t* 
dev_defualt_signalfd(void *data)
{
    dev_event_t *ev_ptr;
    sigset_t mask;  
    int sfd;

    sigemptyset(&mask);  
    sigaddset(&mask, SIGINT);  
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGPIPE);  

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)  
        return NULL;

    sfd = signalfd(-1, &mask, 0);  
    if (sfd == -1) {
        return NULL;
    } 

    ev_ptr = dev_event_creat(sfd, DEV_EVENT_SIG, EPOLLIN, 0);
    if (ev_ptr == NULL) {
        dbg_Print("dev_event_creat\n");
        return NULL;
    }

    dev_event_set_data(ev_ptr, data, dev_signalfd_handler, NULL);

    return ev_ptr;
}


  
 