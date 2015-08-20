# Make sure that D-Bus is running
# D-Bus autolaunch is broken
if test -z "$DBUS_SESSION_BUS_ADDRESS" ; then
    eval `dbus-launch --sh-syntax --exit-with-session`
fi
if $qdbus >/dev/null 2>/dev/null; then
    : # ok
else
    echo 'startkde: Could not start D-Bus. Can you call qdbus?'  1>&2
    test -n "$ksplash_pid" && kill "$ksplash_pid" 2>/dev/null
    ${MESSAGE_CMD} -geometry 500x100 "Could not start D-Bus. Can you call qdbus?"
    exit 1
fi

