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
/*

    int session_id;
    int slot_id;
    int slot_type;
    int board_type;
    long uptime;
    char hw_version[32];
    char sw_version[32];
    long reg_time;
*/

int dev_getenv_int(const char *var)
{
    char *env_ptr = NULL;
    env_ptr = getenv(var);
    if (env_ptr) {
        return atoi(env_ptr);
    } else {
        fprintf(stderr, "%s\n", "can not get env");
    }
    return 0;
}

int 
dev_get_self_info(board_info_t *bif)
{
    bif->slot_id = dev_getenv_int("slotid");
    printf("slot_id = %d\n", bif->slot_id);
    bif->session_id = 0;
    bif->board_type = 0x001;
    bif->slot_type = DEV_STATE_BACKUP;
    bif->uptime = dev_sys_uptime();
    bif->reg_time = 0;
    snprintf(bif->hw_version, sizeof(bif->hw_version), "%s", "v111");
    snprintf(bif->hw_version, sizeof(bif->sw_version), "%s", "v_111");
}

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
 
    rt = malloc(sizeof(struct dev_routine));
    if (rt == NULL) {
        exit(-1);
    }
    rt->timer = dev_event_timer_creat(50, rt);
    dev_get_self_info(&rt->board_info);

    if (rt->board_info.slot_type == DEV_STATE_IO) {
        ev_srv = dev_io_creat(rt);
    } else {
        ev_srv = dev_master_creat(rt);
    }

    if (ev_srv == NULL) {
        fprintf(stderr, "%s\n", "ev_srv is NULL");
        exit(-1);
    }

    dev_event_loop_add(dev_event_deafult_loop(), dev_defualt_signalfd(NULL));
    dev_event_loop_add(dev_event_deafult_loop(), rt->timer);
    dev_event_loop_add(dev_event_deafult_loop(), ev_srv);
    dev_event_loop_run(dev_event_deafult_loop());
    
    return 0;
}
