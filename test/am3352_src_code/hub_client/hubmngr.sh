#!/bin/sh

gw_addr=`route -n | grep eth0 | grep UG | awk '{print $2}'`

mv /etc/user/hub.tar /tmp/
cd /tmp/
tar x -vf hub.tar
cp hub/bin/hub /usr/sbin/
cp -d hub/lib/* /usr/lib/
/usr/sbin/hub ${gw_addr} &

