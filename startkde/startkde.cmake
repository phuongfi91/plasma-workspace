#!/bin/sh
#
#  DEFAULT KDE STARTUP SCRIPT ( @PROJECT_VERSION@ )
#

# Define MESSAGE_CMD as the default message handle
# It can be overrided by custom one
MESSAGE_CMD=xmessage

# Load modular entries instead of monolithic startkde
# Each entry can have similar named one in custom to override it
# Entry 00 is reserved for distro customize variables like MESSAGE_CMD

for i in /etc/startkde.d/*.sh; do
	if [ -r /etc/startkde.d/custom/$(basename $i) ]; then
		. /etc/startkde.d/custom/$(basename $i)
	elif [ -r "$i" ]; then
		. "$i"
	fi
done
