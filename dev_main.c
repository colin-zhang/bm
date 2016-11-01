#include "./def/dev_def.h"
#include "core/dev_event_loop.h"
#include "core/dev_event.h"
#include "core/dev_event_timer.h"
#include "core/dev_signalfd.h"
#include "util/dev_udp.h"
#include "util/dev_if_so.h"
#include "util/dev_utils.h"
#include "util/dev_pid.h"

#include "dev_main.h"
#include <time.h>


//#include "uware_def.h"
//#include "kernel/uware_netlink.h"

int 
devd_tool(int argc, char *argv[])
{
    int opt;
    while( (opt = getopt(argc, argv, "i:p:scb")) != -1) {
        switch(opt) {
        
            break;
        }
    }

    return 0;
}


int main(int argc, char *argv[])
{   
    int slotid = 1, slottype = 1;
    int res = 0;
    char *env_ptr = NULL;
    dev_event_t *ev_srv = NULL;
    struct dev_routine *rt = NULL;

    res = dev_daemon_init("dev2d");
    if (res > 0) {
        return devd_tool(argc, argv);
    }

    if (dev_event_deafult_loop_init(100) == NULL) {
        fprintf(stderr, "Fail to create deafult loop\n");
        exit(-1);
    }

    dev_event_loop_add(dev_event_deafult_loop(), dev_defualt_signalfd(NULL));

    env_ptr = getenv("slotid");
    if (env_ptr) {
        slotid = atoi(env_ptr);
    }
    env_ptr = getenv("slottype");
    if (env_ptr) {
        slottype = atoi(env_ptr);
    }

    rt = malloc(sizeof(struct dev_routine));
    if (rt == NULL) {
        exit(-1);
    }
    
    rt->timer = dev_event_timer_creat(50, rt);
    rt->slot_id = slotid;
    rt->slot_type = slottype;
    rt->start_time = get_timespec_sec();


    if (slotid == 3) {
        ev_srv = dev_io_creat(rt);
    } else {
        ev_srv = dev_master_creat(rt);
    }

    dev_event_loop_add(dev_event_deafult_loop(), rt->timer);
    dev_event_loop_add(dev_event_deafult_loop(), ev_srv);
    dev_event_loop_run(dev_event_deafult_loop());
    
    return 0;
}
