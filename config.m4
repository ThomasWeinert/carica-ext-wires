PHP_ARG_ENABLE(wires,
  [Whether to enable the "wires" extension],
  [  --enable-wires        Enable "wires" extension support])

if test $PHP_WIRES != "no"; then
  PHP_SUBST(WIRES_SHARED_LIBADD)

  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/wiringPi.h"
  if test -r $PHP_WIRES/$SEARCH_FOR; then # path given as parameter
    WIRES_DIR=$PHP_WIRES
  else # search default path list
    AC_MSG_CHECKING([for wires files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        WIRES_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  if test -z "$WIRES_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the wiringpi distribution])
  fi

  PHP_ADD_INCLUDE($WIRES_DIR/include)

  LIBNAME=wiringPi
  LIBSYMBOL=wiringPiSetupGpio

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $WIRES_DIR/lib, WIRES_SHARED_LIBADD)
    AC_DEFINE(HAVE_WIRESLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong wiringPi lib version or lib not found])
  ],[
    -LwiringPi
  ])

  PHP_SUBST(WIRES_SHARED_LIBADD)

  PHP_NEW_EXTENSION(wires, wires.c, $ext_shared)
fi