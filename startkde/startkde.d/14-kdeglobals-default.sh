# Write a default kdeglobals file to set up the font
kdeglobalsfile=$configDir/kdeglobals
test -f $kdeglobalsfile || {
cat >$kdeglobalsfile <<EOF
[General]
XftAntialias=true
XftHintStyle=hintmedium
XftSubPixel=none
EOF
}


