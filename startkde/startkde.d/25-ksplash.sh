ksplash_pid=
if test -z "$dl"; then
  # the splashscreen and progress indicator
  case "$ksplashrc_ksplash_engine" in
    KSplashQML)
      ksplash_pid=`ksplashqml "${ksplashrc_ksplash_theme}" --pid`
      ;;
    None)
      ;;
    *)
      ;;
  esac
fi


