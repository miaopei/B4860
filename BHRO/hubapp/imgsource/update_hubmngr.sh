#!/bin/sh

tar x -vf hub.tar

cp ./hub/bin/hub /usr/sbin/
chmod +x /usr/sbin/hub

cp ./hub/release_hubapp /etc/user/

cp ./hub/user_app_sh /etc/user/
chmod +x /etc/user/user_app_sh

cp -d ./hub/lib/* /usr/lib/


