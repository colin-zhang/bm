PRJ_TARGET = bm_tool
PRJ_TARGET_TYPE = exe

ifndef PRJ_DEBUG
PRJ_DEBUG = no
endif
MK_DIR ?= $(PWD)

PRJ_SRC = 	\
	util/dev_common.c \
	util/dev_udp.c \
	util/dev_if_so.c \
	dev_tool.c \

#PRJ_CFLAG 
PRJ_LDFLAG = 
PRJ_CFLAG =  -Os

include ${MK_DIR}/main.mk
