PWD=`pwd`

WORK_ROOT = $(PWD)

LIBUV = libuv-v1.9.1

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
	#@tar -zxf $(WORK_ROOT)/$(LIBUV).tar.gz -C $(BUILD_DIR)
	#@cd $(BUILD_DIR)/$(LIBUV) && sh autogen.sh && ./configure --host=powerpc64-fsl-linux --prefix=$(WORK_ROOT)/../ && make && make install
	@tar -zxvf $(LIBUV).tar.gz
	@cp -rf $(LIBUV)/* $(BUILD_DIR)
	@cp libstdc++.so.6 ./build/lib
	@tar -zxvf librhub.tar.gz
	@cp -d librhub/*.so* ./build/lib
	@cp librhub/*.h ./build/include
	make -C $(WORK_ROOT)/src
	@make -C $(WORK_ROOT)/hub

install:


.PHONY: clean
clean:
	@echo "[Cleanning ...]"
	@rm -rf $(BUILD_DIR)
	@make clean -C src
	@make clean -C hub
	@rm -rf librhub
	@rm -rf $(LIBUV)

