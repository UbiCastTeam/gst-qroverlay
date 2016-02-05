dnl as-compiler-flag.m4 0.0.1
dnl autostars m4 macro for detection of compiler flags
dnl
dnl ds@schleef.org

AC_DEFUN([AS_COMPILER_FLAG],
[
  AC_MSG_CHECKING([to see if compiler understands $1])

  save_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $1"

  AC_TRY_COMPILE([ ], [], [flag_ok=yes], [flag_ok=no])
  CFLAGS="$save_CFLAGS"

  if test "X$flag_ok" = Xyes ; then
    $2
    true
  else
    $3
    true
  fi
  AC_MSG_RESULT([$flag_ok])
])

AC_DEFUN([AG_GST_CHECK_FEATURE],
[echo
AC_MSG_NOTICE(*** checking feature: [$2] ***)
if test "x[$3]" != "x"
then
  AC_MSG_NOTICE(*** for plug-ins: [$3] ***)
fi
dnl
builtin(define, [gst_endisable], ifelse($5, [disabled], [enable], [disable]))dnl
dnl if it is set to NO, then don't even consider it for building
NOUSE=
if test "x$USE_[$1]" = "xno"; then
  NOUSE="yes"
fi
AC_ARG_ENABLE(translit([$1], A-Z, a-z),
  [  ]builtin(format, --%-26s gst_endisable %s, gst_endisable-translit([$1], A-Z, a-z), [$2]ifelse([$3],,,: [$3])),
  [ case "${enableval}" in
      yes) USE_[$1]=yes;;
      no) USE_[$1]=no;;
      *) AC_MSG_ERROR(bad value ${enableval} for --enable-translit([$1], A-Z, a-z)) ;;
    esac],
  [ USE_$1=]ifelse($5, [disabled], [no], [yes]))           dnl DEFAULT

dnl *** set it back to no if it was preset to no
if test "x$NOUSE" = "xyes"; then
  USE_[$1]="no"
  AC_MSG_WARN(*** $3 pre-configured not to be built)
fi
NOUSE=

dnl *** If it's enabled

if test x$USE_[$1] = xyes; then
  dnl save compile variables before the test

  gst_check_save_LIBS=$LIBS
  gst_check_save_LDFLAGS=$LDFLAGS
  gst_check_save_CFLAGS=$CFLAGS
  gst_check_save_CPPFLAGS=$CPPFLAGS
  gst_check_save_CXXFLAGS=$CXXFLAGS

  HAVE_[$1]=no
  dnl TEST_FOR_FEATURE
  $4

  LIBS=$gst_check_save_LIBS
  LDFLAGS=$gst_check_save_LDFLAGS
  CFLAGS=$gst_check_save_CFLAGS
  CPPFLAGS=$gst_check_save_CPPFLAGS
  CXXFLAGS=$gst_check_save_CXXFLAGS

  dnl If it isn't found, unset USE_[$1]
  if test x$HAVE_[$1] = xno; then
    USE_[$1]=no
  else
    ifelse([$3], , :, [AC_MSG_NOTICE(*** These plugins will be built: [$3])])
  fi
fi
dnl *** Warn if it's disabled or not found
if test x$USE_[$1] = xyes; then
  ifelse([$6], , :, [$6])
  if test "x$3" != "x"; then
    GST_PLUGINS_YES="\t[$3]\n$GST_PLUGINS_YES"
  fi
  AC_DEFINE(HAVE_[$1], , [Define to enable $2]ifelse($3,,, [ (used by $3)]).)
else
  ifelse([$3], , :, [AC_MSG_NOTICE(*** These plugins will not be built: [$3])])
  if test "x$3" != "x"; then
    GST_PLUGINS_NO="\t[$3]\n$GST_PLUGINS_NO"
  fi
  ifelse([$7], , :, [$7])
fi
dnl *** Define the conditional as appropriate
AM_CONDITIONAL(USE_[$1], test x$USE_[$1] = xyes)
])

AC_DEFUN([AG_GST_CHECK_LIBHEADER],
[
  AC_CHECK_LIB([$2], [$3], HAVE_[$1]=yes, HAVE_[$1]=no,[$4])
  if test "x$HAVE_[$1]" = "xyes"; then
    AC_CHECK_HEADER([$5], :, HAVE_[$1]=no)
    if test "x$HAVE_[$1]" = "xyes"; then
      dnl execute what needs to be
      ifelse([$6], , :, [$6])
    else
      ifelse([$7], , :, [$7])
    fi
  else
    ifelse([$7], , :, [$7])
  fi
  AC_SUBST(HAVE_[$1])
]
)

