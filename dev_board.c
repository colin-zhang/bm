#include "./def/dev_def.h"
#include "core/dev_event_loop.h"
#include "core/dev_event.h"
#include "core/dev_event_timer.h"
#include "core/dev_signalfd.h"
#include "util/dev_udp.h"
#include "util/dev_if_so.h"
#include "util/dev_utils.h"
#include "util/dev_pid.h"

#include "dev_board.h"


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
    return 0;
}



