if test "$kdeglobals_kscreen_scalefactor" -ne 1; then
    export QT_DEVICE_PIXEL_RATIO=$kdeglobals_kscreen_scalefactor
fi
