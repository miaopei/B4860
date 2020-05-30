#!/bin/bash

TARGET_BIN="tarFile/hub/bin"
TARGET_LIB="tarFile/hub/lib"

IMGSOURCE=imgsource

[ ! -d "tarFile" ] && mkdir tarFile

[ ! -d ${TARGET_BIN} ] && mkdir -p ${TARGET_BIN}
[ ! -d ${TARGET_LIB} ] && mkdir -p ${TARGET_LIB}

cp ${IMGSOURCE}/release_hubapp tarFile/hub
cp build/bin/hub ${TARGET_BIN}
cp -d build/lib/libuv.so* ${TARGET_LIB}
cp -d build/lib/librhub.so* ${TARGET_LIB}
cp -d build/lib/libbbu.so ${TARGET_LIB}
cp -d build/lib/libftp.so* ${TARGET_LIB}

cd tarFile/ && tar -cvf hub.tar hub && rm -rf hub

