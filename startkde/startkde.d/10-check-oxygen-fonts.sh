# Make sure the Oxygen font is installed
# This is necessary for setups where CMAKE_INSTALL_PREFIX
# is not in /usr. fontconfig looks in /usr, ~/.fonts and
# $XDG_DATA_HOME for fonts. In this case, we symlink the
# Oxygen font under ${XDG_DATA_HOME} and make it known to
# fontconfig

usr_share="/usr/share"
install_share="@CMAKE_INSTALL_PREFIX@/@SHARE_INSTALL_PREFIX@"

if [ ! $install_share = $usr_share ]; then

    if [ ${XDG_DATA_HOME} ]; then
        fontsDir="${XDG_DATA_HOME}/fonts"
    else
        fontsDir="${HOME}/.fonts"
    fi

    test -d $fontsDir || {
        mkdir -p $fontsDir
    }

    oxygenDir=$fontsDir/oxygen
    prefixDir="@CMAKE_INSTALL_PREFIX@/@SHARE_INSTALL_PREFIX@/fonts/oxygen"

    # if the oxygen dir doesn't exist, create a symlink to be sure that the
    # Oxygen font is available to the user
    test -d $oxygenDir || test -d $prefixDir && {
        test -h $oxygenDir || ln -s $prefixDir $oxygenDir && fc-cache $oxygenDir
    }
fi

