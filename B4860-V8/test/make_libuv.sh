#!/bin/bash

work_root=$(pwd)
mkdir -p ${work_root}/build_libuv

cc_path=/home/miaopei/anaconda2/bin/gcc
install_path=${work_root}/build_libuv

echo ${cc_path}
echo ${install_pathl}

tar -zxvf ${work_root}/libuv-v1.9.1.tar.gz

cd ${work_root}/libuv-v1.9.1 && \
    sh autogen.sh && \
    ./configure CC=${cc_path} -–prefix=${install_path} && \
    make && \
    make install
#make check

