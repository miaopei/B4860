PWD=`pwd`

UNAME := $(shell uname -s)

COMPILE_TIME = $(shell date +"%Y-%M-%d %H:%M:%S")

WORK_ROOT = $(PWD)

LIBUV = libuv-v1.9.1

BUILD_DIR = $(WORK_ROOT)/build
INCLUDE_DIR = $(BUILD_DIR)/include
LIB_DIR = $(BUILD_DIR)/lib
BIN_DIR = $(BUILD_DIR)/bin

LD_INCLUDE_PATH = $(INCLUDE_DIR)
LD_LIBRARY_PATH = $(LIB_DIR)

all: before bbu
	@echo "[build done ...]"
	@make -C ftplibpp
	@make -C ftplibpp install
	@make -C hub
	@make -C rru
	@make -C oam
	@echo "[compile time $(COMPILE_TIME)]"

before:
	@test -d $(BUILD_DIR) || mkdir -p $(BUILD_DIR)
	@test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)

libuv:
	@echo "[build libuv ...]"
	@tar -zxf $(WORK_ROOT)/$(LIBUV).tar.gz -C $(BUILD_DIR)
	#@cd $(BUILD_DIR)/$(LIBUV) && sh autogen.sh && ./configure --prefix=$(WORK_ROOT)/../ && make && make install
ifeq ($(UNAME), Linux)
	@cd $(BUILD_DIR)/$(LIBUV) && sh autogen.sh && ./configure --prefix=$(WORK_ROOT)/../ && make && make install
endif
	
ifeq ($(UNAME), Darwin)
	@cd $(BUILD_DIR)/$(LIBUV) && ./gyp_uv.py -f xcode && xcodebuild -ARCHS="x86_64" -project out/uv.xcodeproj -configuration Release -alltargets
endif

src: libuv
	make -C $(WORK_ROOT)/src

bbu: src
	make -C $(WORK_ROOT)/bbu

.PHONY: clean
clean:
	@echo "[Cleanning ...]"
	@rm -rf $(BUILD_DIR)
	@make clean -C src
	@make clean -C bbu
	@make -C ftplibpp clean
	@make clean -C hub
	@make clean -C rru
	@make clean -C oam

