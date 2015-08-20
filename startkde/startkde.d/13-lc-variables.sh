# export LC_* variables set by kcmshell5 formats into environment
# so it can be picked up by QLocale and friends.
exportformatssettings=$configDir/plasma-locale-settings.sh
test -f $exportformatssettings && {
    . $exportformatssettings
}


