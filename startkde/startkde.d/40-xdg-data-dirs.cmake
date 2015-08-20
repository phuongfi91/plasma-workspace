# Make sure that the KDE prefix is first in XDG_DATA_DIRS and that it's set at all.
# The spec allows XDG_DATA_DIRS to be not set, but X session startup scripts tend
# to set it to a list of paths *not* including the KDE prefix if it's not /usr or
# /usr/local.
if test -z "$XDG_DATA_DIRS"; then
    XDG_DATA_DIRS="@CMAKE_INSTALL_PREFIX@/@SHARE_INSTALL_PREFIX@:/usr/share:/usr/local/share"
fi
export XDG_DATA_DIRS
