#include "core/dev_event_loop.h"
#include "core/dev_event.h"
#include "core/dev_event_timer.h"
#include "core/dev_signalfd.h"
#include "util/dev_pid.h"

#include "dev_board.h"
#include "dev_master.h"
#include "dev_io.h"

#include <signal.h>
#include <execinfo.h>

void backtrace_print(int sig_num)
{
    int i = 0;
    void* buffer[100] = {0};
    int n = backtrace(buffer, 100);
    char** symbols = (char**)backtrace_symbols(buffer, n);

    printf("%s\n", strsignal(sig_num));

    if (symbols != NULL) {
        for(i = 0; i < n; i++) {
            printf("%p: %s\n", buffer[i], symbols[i]);
        }
        fflush(stdout);
    }

    exit(-1);
}

void siginit(void) 
{
    signal(SIGSEGV , backtrace_print);
    signal(SIGFPE , backtrace_print);    
    signal(SIGILL , backtrace_print);
    signal(SIGBUS , backtrace_print);
    signal(SIGABRT , backtrace_print);
}

static int 
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
    int res = 0, slottype = 0;
    char *env_ptr = NULL;
    dev_event_t *ev_srv = NULL;
    dev_routine_t *rt = NULL;

    siginit();

    res = dev_daemon_init("dev2d");
    if (res > 0) {
        return devd_tool(argc, argv);
    }

    if (dev_event_deafult_loop_init(10) == NULL) {
        fprintf(stderr, "Fail to create deafult loop\n");
        exit(-1);
    }
 
    rt = dev_board_rt_init(&slottype);
    if (rt == NULL) {
        fprintf(stderr, "%s\n", "dev_board_rt_init is fail");
        exit(-1);
    }

    switch(slottype) {
        case DEV_STATE_IO:
            ev_srv = dev_io_creat(rt);
            break;
        case DEV_STATE_BACKUP:
            ev_srv = dev_master_creat(rt);
            break;
    }

    if (ev_srv == NULL) {
        fprintf(stderr, "%s\n", "ev_srv is NULL");
        exit(-1);
    }
    //dev_event_loop_add(dev_event_deafult_loop(), dev_defualt_signalfd(NULL));
    dev_event_loop_add(dev_event_deafult_loop(), rt->timer);
    dev_event_loop_add(dev_event_deafult_loop(), rt->board_api);
    dev_event_loop_add(dev_event_deafult_loop(), ev_srv);
    dev_event_loop_run(dev_event_deafult_loop());
    return 0;
}
