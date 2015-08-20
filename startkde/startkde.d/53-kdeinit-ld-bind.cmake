# We set LD_BIND_NOW to increase the efficiency of kdeinit.
# kdeinit unsets this variable before loading applications.
LD_BIND_NOW=true @CMAKE_INSTALL_FULL_LIBEXECDIR_KF5@/start_kdeinit_wrapper --kded +kcminit_startup
if test $? -ne 0; then
  # Startup error
  echo 'startkde: Could not start kdeinit5. Check your installation.'  1>&2
  test -n "$ksplash_pid" && kill "$ksplash_pid" 2>/dev/null
  ${MESSAGE_CMD} -geometry 500x100 "Could not start kdeinit5. Check your installation."
  exit 1
fi
