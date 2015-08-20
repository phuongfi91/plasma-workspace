# At this point all environment variables are set, let's send it to the DBus session server to update the activation environment
@CMAKE_INSTALL_FULL_LIBEXECDIR@/ksyncdbusenv
if test $? -ne 0; then
  # Startup error
  echo 'startkde: Could not sync environment to dbus.'  1>&2
  test -n "$ksplash_pid" && kill "$ksplash_pid" 2>/dev/null
  ${MESSAGE_CMD} -geometry 500x100 "Could not sync environment to dbus."
  exit 1
fi


