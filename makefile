all:dev tool

dev:
	make -f $(SRC_DIR)/bm.mk
tool:
	make -f $(SRC_DIR)/tool.mk