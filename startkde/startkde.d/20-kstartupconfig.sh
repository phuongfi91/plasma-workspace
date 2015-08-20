# Boot sequence:
#
# kdeinit is used to fork off processes which improves memory usage
# and startup time.
#
# * kdeinit starts klauncher first.
# * Then kded is started. kded is responsible for keeping the sycoca
#   database up to date. When an up to date database is present it goes
#   into the background and the startup continues.
# * Then kdeinit starts kcminit. kcminit performs initialisation of
#   certain devices according to the user's settings
#
# * Then ksmserver is started which takes control of the rest of the startup sequence

echo 'startkde: Starting up...'  1>&2

kstartupconfig5
returncode=$?
if test $returncode -ne 0; then
    ${MESSAGE_CMD} -geometry 500x100 "kstartupconfig5 does not exist or fails. The error code is $returncode. Check your installation."
    exit 1
fi
[ -r $configDir/startupconfig ] && . $configDir/startupconfig


