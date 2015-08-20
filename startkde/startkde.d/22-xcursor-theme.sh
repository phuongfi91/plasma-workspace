# XCursor mouse theme needs to be applied here to work even for kded or ksmserver
if test -n "$kcminputrc_mouse_cursortheme" -o -n "$kcminputrc_mouse_cursorsize" ; then
    @EXPORT_XCURSOR_PATH@

    kapplymousetheme "$kcminputrc_mouse_cursortheme" "$kcminputrc_mouse_cursorsize"
    if test $? -eq 10; then
        XCURSOR_THEME=breeze_cursors
        export XCURSOR_THEME
    elif test -n "$kcminputrc_mouse_cursortheme"; then
        XCURSOR_THEME="$kcminputrc_mouse_cursortheme"
        export XCURSOR_THEME
    fi
    if test -n "$kcminputrc_mouse_cursorsize"; then
        XCURSOR_SIZE="$kcminputrc_mouse_cursorsize"
        export XCURSOR_SIZE
    fi
fi

# Set a left cursor instead of the standard X11 "X" cursor, since I've heard
# from some users that they're confused and don't know what to do. This is
# especially necessary on slow machines, where starting KDE takes one or two
# minutes until anything appears on the screen.
#
# If the user has overwritten fonts, the cursor font may be different now
# so don't move this up.
#
xsetroot -cursor_name left_ptr

