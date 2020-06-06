#!/bin/sh

BBUAPPLOG=/var/log/bbuapp.log

check_str='*DSP Booted up*'
filter=`cat /var/log/l1.log | grep "DSP Booted up"`

while [[ -z ${filter} || ${filter} !=  ${check_str} ]]
do
	sleep 3
	filter=`cat /var/log/l1.log | grep "DSP Booted up"`
	echo "l1.log filter result ${filter}" > ${BBUAPPLOG}
done

while true;do
	cpri0Status=`devmem 0xffe880004 32`
    if [ $cpri0Status == "0x0000080E" ];then
		echo "cpri status ok!" >> ${BBUAPPLOG}
        break
    fi
    sleep 5
done

insmod /usr/driver/ceth.ko
ifconfig ceth 10.0.0.1 netmask 255.255.255.0

/etc/init.d/dnsmasq start

export LD_LIBRARY_PATH=:/mnt/fap/lib/

/mnt/fap/bin/bbuapp > /var/log/bbuapp.log 2>&1 &

