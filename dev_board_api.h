#ifndef _DEV_BOARD_API_H_
#define _DEV_BOARD_API_H_

#include <stdint.h>
#include "core/dev_event.h"
#include "dev_protocol.h"
#include "dev_board.h"
#include "dev_lib.h"

dev_event_t *dev_board_api_init(void *data);

#endif