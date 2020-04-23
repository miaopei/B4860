#!/bin/sh

tar x -vf hub.tar

cp ./hub/bin/hub /usr/sbin/
chmod +x /usr/sbin/hub

cp ./hub/release_hubapp /etc/user/

cp -d ./hub/lib/* /usr/lib/

