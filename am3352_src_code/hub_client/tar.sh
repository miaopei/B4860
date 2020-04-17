#!/bin/bash

TARGET_BIN="tarFile/hub/bin"
TARGET_LIB="tarFile/hub/lib"

[ ! -d "tarFile" ] && mkdir tarFile

[ ! -d "tarFile/hub/bin" ] && mkdir -p tarFile/hub/bin
[ ! -d "tarFile/hub/lib" ] && mkdir -p tarFile/hub/lib

cp hubmngr.sh tarFile/hub
cp build/bin/hub ${TARGET_BIN}
cp -d build/lib/libuv.so* ${TARGET_LIB}
cp -d build/lib/librhub.so* ${TARGET_LIB}
cp -d build/lib/libbbu.so ${TARGET_LIB}
cp -d build/lib/libftp.so* ${TARGET_LIB}

cp -rf rhup tarFile/hub

cd tarFile && tar -cvf hub.tar hub 

