# We need to create config folder so we can write startupconfigkeys
if [  ${XDG_CONFIG_HOME} ]; then
  configDir=$XDG_CONFIG_HOME;
else
  configDir=${HOME}/.config; #this is the default, http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
fi

mkdir -p $configDir


