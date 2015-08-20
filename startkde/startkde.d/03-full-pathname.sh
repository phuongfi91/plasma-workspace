# in case we have been started with full pathname spec without being in PATH
bindir=`echo "$0" | sed -n 's,^\(/.*\)/[^/][^/]*$,\1,p'`
if [ -n "$bindir" ]; then
  qbindir=`qtpaths --binaries-dir`
  qdbus=$qbindir/qdbus
  case $PATH in
    $bindir|$bindir:*|*:$bindir|*:$bindir:*) ;;
    *) PATH=$bindir:$PATH; export PATH;;
  esac
else
  qdbus=qdbus
fi


