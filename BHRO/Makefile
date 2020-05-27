PWD=`pwd`

UNAME := $(shell uname -s)

COMPILE_TIME = $(shell date +"%Y-%M-%d %H:%M:%S")

WORK_ROOT = $(PWD)

LIBUV = libuv-v1.9.1

PLATFORM = linux-gcc

BUILD_DIR = $(WORK_ROOT)/build
INCLUDE_DIR = $(BUILD_DIR)/include
LIB_DIR = $(BUILD_DIR)/lib
BIN_DIR = $(BUILD_DIR)/bin

LD_INCLUDE_PATH = $(INCLUDE_DIR)
LD_LIBRARY_PATH = $(LIB_DIR)

all: before bbu
	@echo "[build ftplibpp ...]"
	@make -C ftplibpp
	@make -C ftplibpp install
	@echo "[build hub ...]"
	@make -C hub_test
	@echo "[build rru ...]"
	@make -C rru_test
	@echo "[build oam_adapter ...]"
	@make -C oam_adapter
	@echo "[build adapter_cli ...]"
	@make -C adapter_cli
	@echo "[build test ...]"
	@make -C test
	@echo "[build done ...]"
	@echo "[compile time $(COMPILE_TIME)]"

before:
	@echo "[build env path ...]"
	@test -d $(BUILD_DIR) || mkdir -p $(BUILD_DIR)
	@test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)

libuv:
	@echo "[build libuv ...]"
	@tar -zxf $(WORK_ROOT)/$(LIBUV).tar.gz -C $(BUILD_DIR)
	@cd $(BUILD_DIR)/$(LIBUV) && sh autogen.sh && ./configure --prefix=$(WORK_ROOT)/../ && make && make install

src: libuv
	@echo "[build src ...]"
	make -C $(WORK_ROOT)/src

bbu: src
	@echo "[build bbu ...]"
	make -C $(WORK_ROOT)/bbuapp

.PHONY: clean
clean:
	@echo "[Cleanning ...]"
	@rm -rf $(BUILD_DIR)
	@make clean -C src
	@make clean -C bbuapp
	@make clean -C hub_test
	@make clean -C rru_test
	@make clean -C test
	@make clean -C oam_adapter
	@make clean -C adapter_cli

