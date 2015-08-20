#This is basically setting defaults so we can use them with kstartupconfig5
cat >$configDir/startupconfigkeys <<EOF
kcminputrc Mouse cursorTheme 'breeze_cursors'
kcminputrc Mouse cursorSize ''
ksplashrc KSplash Theme Breeze
ksplashrc KSplash Engine KSplashQML
kdeglobals KScreen ScaleFactor 1
kcmfonts General forceFontDPI 0
EOF


