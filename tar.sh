#!/bin/bash

tar -zcvf ${1}.tar.gz ${1} 

mv ${1}.tar.gz ${1}.tar.gc

tar -zcvf ${1}.tar.gc.tar.gz ${1}.tar.gc