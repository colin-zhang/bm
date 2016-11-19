PRJ_TARGET = bmd
PRJ_TARGET_TYPE = exe

ifndef PRJ_DEBUG
PRJ_DEBUG = no
endif
MK_DIR ?= $(PWD)

PRJ_SRC = 	\
	core/dev_heap.c \
	core/dev_event.c \
	core/dev_event_loop.c \
	core/dev_event_timer.c \
	core/dev_signalfd.c \
	util/dev_common.c \
	util/dev_ipc.c \
	util/dev_udp.c \
	util/dev_pid.c \
	util/dev_if_so.c \
	util/dev_utils.c \
	util/dev_log.c \
	dev_master.c \
	dev_io.c \
	dev_protocol.c \
	dev_board.c \
	dev_main.c \
	dev_board_api.c \
	

#PRJ_CFLAG 
PRJ_LDFLAG = -lrt 
PRJ_CFLAG = -D UWARE_DEFAULT_IP_DOMAIN=\"192.168.11.0\" 

include ${MK_DIR}/main.mk
