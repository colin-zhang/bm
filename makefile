SRC_DIR?=.
all:dev tool

dev:
	make -f $(SRC_DIR)/bm.mk
tool:
	make -f $(SRC_DIR)/tool.mk
	make -f $(SRC_DIR)/tool.mk

clean:
	make -f $(SRC_DIR)/bm.mk clean
	make -f $(SRC_DIR)/tool.mk clean