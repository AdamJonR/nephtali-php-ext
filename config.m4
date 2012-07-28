dnl $Id$
dnl config.m4 for extension nephtali

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(nephtali, for nephtali support,
dnl Make sure that the comment is aligned:
dnl [  --with-nephtali             Include nephtali support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(nephtali, whether to enable nephtali support,
Make sure that the comment is aligned:
[  --enable-nephtali           Enable nephtali support])

if test "$PHP_NEPHTALI" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-nephtali -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/nephtali.h"  # you most likely want to change this
  dnl if test -r $PHP_NEPHTALI/$SEARCH_FOR; then # path given as parameter
  dnl   NEPHTALI_DIR=$PHP_NEPHTALI
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for nephtali files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       NEPHTALI_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$NEPHTALI_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the nephtali distribution])
  dnl fi

  dnl # --with-nephtali -> add include path
  dnl PHP_ADD_INCLUDE($NEPHTALI_DIR/include)

  dnl # --with-nephtali -> check for lib and symbol presence
  dnl LIBNAME=nephtali # you may want to change this
  dnl LIBSYMBOL=nephtali # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $NEPHTALI_DIR/lib, NEPHTALI_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_NEPHTALILIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong nephtali lib version or lib not found])
  dnl ],[
  dnl   -L$NEPHTALI_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(NEPHTALI_SHARED_LIBADD)

  PHP_NEW_EXTENSION(nephtali, nephtali.c, $ext_shared)
fi
