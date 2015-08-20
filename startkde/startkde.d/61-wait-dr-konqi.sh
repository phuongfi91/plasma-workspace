wait_drkonqi=`kreadconfig5 --file startkderc --group WaitForDrKonqi --key Enabled --default true`

if test x"$wait_drkonqi"x = x"true"x ; then
    # wait for remaining drkonqi instances with timeout (in seconds)
    wait_drkonqi_timeout=`kreadconfig5 --file startkderc --group WaitForDrKonqi --key Timeout --default 900`
    wait_drkonqi_counter=0
    while $qdbus | grep "^[^w]*org.kde.drkonqi" > /dev/null ; do
        sleep 5
        wait_drkonqi_counter=$((wait_drkonqi_counter+5))
        if test "$wait_drkonqi_counter" -ge "$wait_drkonqi_timeout" ; then
            # ask remaining drkonqis to die in a graceful way
            $qdbus | grep 'org.kde.drkonqi-' | while read address ; do
                $qdbus "$address" "/MainApplication" "quit"
            done
            break
        fi
    done
fi
