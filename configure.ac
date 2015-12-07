AC_INIT([mqmx], [0.0.0], [oleg.peregudov@nokia.com])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PROG_CXX

AM_PROG_AR
AM_PROG_CC_C_O

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

LT_INIT([shared])

MQMX_CURRENT=0
MQMX_REVISION=0
MQMX_AGE=0

MQMX_LT_VERSION="${MQMX_CURRENT}:${MQMX_REVISION}:${MQMX_AGE}"

AC_SUBST(MQMX_LT_VERSION)
AC_SUBST(MQMX_CURRENT)
AC_SUBST(MQMX_REVISION)
AC_SUBST(MQMX_AGE)

dnl
dnl check if compiler can be pedantic
dnl
AC_MSG_CHECKING([if $CXX supports -pedantic])
ac_pedantic_supported=no
save_CXXFLAGS=$CXXFLAGS
CXXFLAGS="$CXXFLAGS -pedantic"
AC_LANG_PUSH([C++])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],
  [ac_pedantic_supported=yes],
  [ac_pedantic_supported=no])
AC_LANG_POP([C++])
CXXFLAGS="$save_CXXFLAGS"
AC_MSG_RESULT([$ac_pedantic_supported])
if test x$ac_pedantic_supported = xyes; then
  CXXFLAGS="$CXXFLAGS -pedantic"
fi

dnl
dnl check if compiler supports -Wall -Werror switches
dnl
AC_MSG_CHECKING([if $CXX supports -Wall -Werror])
ac_report_all_warnings_supported=no
save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -Wall -Werror"
AC_LANG_PUSH([C++])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],
  [ac_report_all_warnings_supported=yes],
  [ac_report_all_warnings_supported=no])
AC_LANG_POP([C++])
CXXFLAGS="$save_CXXFLAGS"
AC_MSG_RESULT([$ac_report_all_warnings_supported])
if test x$ac_report_all_warnings_supported = xyes; then
  CXXFLAGS="$CXXFLAGS -Wall -Werror"
fi

dnl
dnl check if compiler supports -std=c++14
dnl
AC_MSG_CHECKING([if $CXX supports -std=c++14])
ac_std_cxx_14_supported=no
save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -std=c++14"
AC_LANG_PUSH([C++])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],
  [ac_std_cxx_14_supported=yes],
  [ac_std_cxx_14_supported=no])
AC_LANG_POP([C++])
CXXFLAGS="$save_CXXFLAGS"
AC_MSG_RESULT([$ac_std_cxx_14_supported])
if test x$ac_std_cxx_14_supported = xyes; then
  CXXFLAGS="$CXXFLAGS -std=c++14"
else
  dnl
  dnl check if compiler supports -std=c++11
  dnl
  AC_MSG_CHECKING([if $CXX supports -std=c++11])
  ac_std_cxx_11_supported=no
  save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -std=c++11"
  AC_LANG_PUSH([C++])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],
    [ac_std_cxx_11_supported=yes],
    [ac_std_cxx_11_supported=no])
  AC_LANG_POP([C++])
  CXXFLAGS="$save_CXXFLAGS"
  AC_MSG_RESULT([$ac_std_cxx_11_supported])
  if test x$ac_std_cxx_11_supported = xyes; then
    CXXFLAGS="$CXXFLAGS -std=c++11"
  else
    AC_MSG_FAILURE([ISO C++ 14 or 11 is required])
  fi
fi

AX_PTHREAD([ac_pthread_lib_found=yes],
           [ac_pthread_lib_found=no])
if test x$ac_pthread_lib_found = xyes; then
  CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
  CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
  LIBS="$PTHREAD_LIBS $LIBS"
fi

AX_OPENMP([ac_openmp_supported=yes],
          [ac_openmp_supported=no])
if test x$ac_openmp_supported = xyes; then
  CFLAGS="$CFLAGS $OPENMP_CFLAGS"
  CXXFLAGS="$CXXFLAGS $OPENMP_CFLAGS"
fi

AC_CONFIG_FILES([
    Makefile
    mqmx/Makefile
    test/Makefile
])
AC_OUTPUT
