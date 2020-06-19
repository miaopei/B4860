#!/bin/sh

echo "update system debug cmd file............"
if [ ! -f "/tmp/rhub/user_app_sh" ]; then
	echo "not fount system debug cmd file............."
else
	echo "copy system debug cmd file to /etc/user............."
	cp /tmp/rhub/user_app_sh /etc/user
    chmod 777 /etc/user/user_app_sh
fi

