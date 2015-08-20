# finally, give the session control to the session manager
# see kdebase/ksmserver for the description of the rest of the startup sequence
# if the KDEWM environment variable has been set, then it will be used as KDE's
# window manager instead of kwin.
# if KDEWM is not set, ksmserver will ensure kwin is started.
# kwrapper5 is used to reduce startup time and memory usage
# kwrapper5 does not return useful error codes such as the exit code of ksmserver.
# We only check for 255 which means that the ksmserver process could not be
# started, any problems thereafter, e.g. ksmserver failing to initialize,
# will remain undetected.
test -n "$KDEWM" && KDEWM="--windowmanager $KDEWM"
# If the session should be locked from the start (locked autologin),
# lock now and do the rest of the KDE startup underneath the locker.
KSMSERVEROPTIONS=""
test -n "$dl" && KSMSERVEROPTIONS=" --lockscreen"
kwrapper5 ksmserver $KDEWM $KSMSERVEROPTIONS
if test $? -eq 255; then
  # Startup error
  echo 'startkde: Could not start ksmserver. Check your installation.'  1>&2
  test -n "$ksplash_pid" && kill "$ksplash_pid" 2>/dev/null
  ${MESSAGE_CMD} -geometry 500x100 "Could not start ksmserver. Check your installation."
fi
