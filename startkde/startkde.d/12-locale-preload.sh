# preload the user's locale on first start
plasmalocalerc=$configDir/plasma-localerc
test -f $plasmalocalerc || {
cat >$plasmalocalerc <<EOF
[Formats]
LANG=$LANG
EOF
}


