#include "./def/dev_def.h"
#include "core/dev_event_loop.h"
#include "core/dev_event.h"
#include "core/dev_event_timer.h"
#include "core/dev_signalfd.h"
#include "util/dev_udp.h"
#include "util/dev_if_so.h"
#include "util/dev_utils.h"
#include "util/dev_log.h"

#include "dev_board_api.h"

int 
dev_board_common_disp(dev_routine_t *rt, int type)
{
    switch (type) {
        case DEV_RPROBE:
            break;
        case DEV_REGISTER:
            break;
    }

    return 0;
}
