# Source scripts found in <config locations>/plasma-workspace/env/*.sh
# (where <config locations> correspond to the system and user's configuration
# directories, as identified by Qt's qtpaths,  e.g.  $HOME/.config
# and /etc/xdg/ on Linux)
#
# This is where you can define environment variables that will be available to
# all KDE programs, so this is where you can run agents using e.g. eval `ssh-agent`
# or eval `gpg-agent --daemon`.
# Note: if you do that, you should also put "ssh-agent -k" as a shutdown script
#
# (see end of this file).
# For anything else (that doesn't set env vars, or that needs a window manager),
# better use the Autostart folder.

# TODO: Use GenericConfigLocation once we depend on Qt 5.4
scriptpath=`qtpaths --paths ConfigLocation | tr ':' '\n' | sed 's,$,/plasma-workspace,g'`

# Add /env/ to the directory to locate the scripts to be sourced
for prefix in `echo $scriptpath`; do
  for file in "$prefix"/env/*.sh; do
    test -r "$file" && . "$file"
  done
done


