echo 'startkde: Shutting down...'  1>&2
# just in case
test -n "$ksplash_pid" && kill "$ksplash_pid" 2>/dev/null

# Clean up
kdeinit5_shutdown

echo 'startkde: Running shutdown scripts...'  1>&2

# Run scripts found in <config locations>/plasma-workspace/shutdown
for prefix in `echo "$scriptpath"`; do
  for file in `ls "$prefix"/shutdown 2> /dev/null | egrep -v '(~|\.bak)$'`; do
    test -x "$prefix/shutdown/$file" && "$prefix/shutdown/$file"
  done
done

unset KDE_FULL_SESSION
xprop -root -remove KDE_FULL_SESSION
unset KDE_SESSION_VERSION
xprop -root -remove KDE_SESSION_VERSION
unset KDE_SESSION_UID

echo 'startkde: Done.'  1>&2
