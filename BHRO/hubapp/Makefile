PWD=`pwd`

WORK_ROOT = $(PWD)

LIBUV = libuv-v1.9.1

LIBSOURCE = libsource

BUILD_DIR = $(WORK_ROOT)/build
INCLUDE_DIR = $(BUILD_DIR)/include
LIB_DIR = $(BUILD_DIR)/lib
BIN_DIR = $(BUILD_DIR)/bin

LD_INCLUDE_PATH = $(INCLUDE_DIR)
LD_LIBRARY_PATH = $(LIB_DIR)

all: 
	@test -d $(BUILD_DIR) || mkdir -p $(BUILD_DIR)
	@test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)
	@echo "[build libuv ...]"
	@tar -zxvf $(LIBSOURCE)/$(LIBUV).tar.gz -C $(BUILD_DIR)
	@cp -rf $(BUILD_DIR)/$(LIBUV)/* $(BUILD_DIR)
	@echo "[build rhub ...]"
	@tar -zxvf $(LIBSOURCE)/librhub.tar.gz -C $(BUILD_DIR)
	@cp -d $(BUILD_DIR)/librhub/*.so* $(LIB_DIR)
	@cp $(BUILD_DIR)/librhub/*.h $(INCLUDE_DIR)
	@echo "[build ftplibpp ...]"
	@make -C $(WORK_ROOT)/ftplibpp
	@make -C $(WORK_ROOT)/ftplibpp install
	@echo "[build src ...]"
	@cp -rf ../src .
	@make clean -C src
	@cp make_src.mk ./src/Makefile
	@make -C $(WORK_ROOT)/src
	@echo "[build hub ...]"
	@make -C $(WORK_ROOT)/hub
	@./tar.sh

install:


.PHONY: clean
clean:
	@echo "[Cleanning ...]"
	@rm -rf $(BUILD_DIR)
	@make clean -C hub
	@rm -rf librhub
	@rm -rf $(LIBUV)
	@make -C $(WORK_ROOT)/ftplibpp clean
	@rm -rf tarFile
	@rm -rf src

