#!/bin/bash

TARGET_BIN="tarFile/hubapp/hub/bin"
TARGET_LIB="tarFile/hubapp/hub/lib"

IMGSOURCE=imgsource

[ ! -d "tarFile" ] && mkdir tarFile

[ ! -d "tarFile/hubapp/hub/bin" ] && mkdir -p tarFile/hubapp/hub/bin
[ ! -d "tarFile/hubapp/hub/lib" ] && mkdir -p tarFile/hubapp/hub/lib

cp ${IMGSOURCE}/update_hubmngr.sh tarFile/hubapp
cp ${IMGSOURCE}/release_hubapp tarFile/hubapp/hub
cp ${IMGSOURCE}/user_app_sh tarFile/hubapp/hub

cp build/bin/hub ${TARGET_BIN}
cp -d build/lib/libuv.so* ${TARGET_LIB}
cp -d build/lib/librhub.so* ${TARGET_LIB}
cp -d build/lib/libbbu.so ${TARGET_LIB}
cp -d build/lib/libftp.so* ${TARGET_LIB}

cd tarFile/hubapp && tar -cvf hub.tar hub && rm -rf hub
cd ../ && tar -cvf hubapp.tar hubapp


