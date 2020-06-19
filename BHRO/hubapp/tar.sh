#!/bin/bash

TARGET_BIN="tarFile/hub/bin"
TARGET_LIB="tarFile/hub/lib"

IMGSOURCE=imgsource

[ ! -d "tarFile" ] && mkdir tarFile

[ ! -d ${TARGET_BIN} ] && mkdir -p ${TARGET_BIN}
[ ! -d ${TARGET_LIB} ] && mkdir -p ${TARGET_LIB}

cp ${IMGSOURCE}/release_hubapp tarFile/hub
cp build/bin/hubapp ${TARGET_BIN}
cp -d build/lib/libuv.so* ${TARGET_LIB}
cp -d build/lib/librhub.so* ${TARGET_LIB}
cp -d build/lib/libbbu.so ${TARGET_LIB}
cp -d build/lib/libftp.so* ${TARGET_LIB}

cd tarFile/ && tar -cvf hub.tar hub && rm -rf hub && cd ..

mkdir -p tarFile/rhub
cp ${IMGSOURCE}/jinsha_fpga_top.bit tarFile/rhub/
cp ${IMGSOURCE}/update_cmd_file.sh tarFile/rhub/
cp ${IMGSOURCE}/user_app_sh tarFile/rhub/

cd tarFile && tar -cvf ${1}.IMG hub.tar rhub && cd ..

