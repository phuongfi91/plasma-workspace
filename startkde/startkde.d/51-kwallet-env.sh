# At this point all the environment is ready, let's send it to kwalletd if running
if test -n "$PAM_KWALLET_LOGIN" ; then
    env | socat STDIN UNIX-CONNECT:$PAM_KWALLET_LOGIN
fi
# ...and also to kwalletd5
if test -n "$PAM_KWALLET5_LOGIN" ; then
    env | socat STDIN UNIX-CONNECT:$PAM_KWALLET5_LOGIN
fi
