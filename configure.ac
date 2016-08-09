AC_INIT([mqmx], [0.0.0], [oleg.peregudov@gmail.com])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects tar-pax])
AM_SILENT_RULES([yes])

AM_PROG_AR
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

LT_INIT([shared])

MQMX_CURRENT=0
MQMX_REVISION=0
MQMX_AGE=0

MQMX_LT_VERSION="${MQMX_CURRENT}:${MQMX_REVISION}:${MQMX_AGE}"

AC_SUBST(MQMX_LT_VERSION)
AC_SUBST(MQMX_CURRENT)
AC_SUBST(MQMX_REVISION)
AC_SUBST(MQMX_AGE)

AC_PATH_PROG(PKG_CONFIG, pkg-config${EXEEXT}, no)
if test "x$PKG_CONFIG" != xno ; then
  if $PKG_CONFIG --atleast-pkgconfig-version 0.7 ; then
    :
  else
    AC_MSG_FAILURE([pkg-config too old; version 0.7 or better required.])
  fi
else
  AC_MSG_FAILURE([pkg-config is required. Please install and run configure once again.])
fi

dnl
dnl doxygen
dnl
AC_PATH_PROG(DOXYGEN, doxygen${EXEEXT}, no)
if test "x$DOXYGEN" = xno ; then
  DOXYGEN=
fi
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])

dnl
dnl gcov/lcov stuff
dnl
AX_CODE_COVERAGE
if test $CODE_COVERAGE_ENABLED = yes; then
  CFLAGS="$CODE_COVERAGE_CFLAGS"
  CXXFLAGS="$CODE_COVERAGE_CFLAGS"
  CPPFLAGS="$CODE_COVERAGE_CPPFLAGS"
  LIBS="$CODE_COVERAGE_LDFLAGS"
fi

dnl
dnl Check for supported C++ standard
dnl
HAVE_CXX11=0
HAVE_CXX14=0
AX_CXX_COMPILE_STDCXX([14], [noext], [optional])
if test $HAVE_CXX14 -ne 1; then
  AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])
else
  HAVE_CXX11=1
fi
AC_SUBST(HAVE_CXX11)
AC_SUBST(HAVE_CXX14)

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
dnl check if compiler supports -Wall -Wextra -Werror switches
dnl
AC_MSG_CHECKING([if $CXX supports -Wall -Wextra -Werror])
ac_report_all_warnings_supported=no
save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -Wall -Wextra -Werror"
AC_LANG_PUSH([C++])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],
  [ac_report_all_warnings_supported=yes],
  [ac_report_all_warnings_supported=no])
AC_LANG_POP([C++])
CXXFLAGS="$save_CXXFLAGS"
AC_MSG_RESULT([$ac_report_all_warnings_supported])
if test x$ac_report_all_warnings_supported = xyes; then
  CXXFLAGS="$CXXFLAGS -Wall -Wextra -Werror"
fi

dnl
dnl check for POSIX thread
dnl
AX_PTHREAD([ac_pthread_lib_found=yes],
           [ac_pthread_lib_found=no])
if test x$ac_pthread_lib_found = xyes; then
  CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
  CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
  LIBS="$PTHREAD_LIBS $LIBS"
fi

dnl
dnl check for OpenMP
dnl
AX_OPENMP([ac_openmp_supported=yes],
          [ac_openmp_supported=no])
if test x$ac_openmp_supported = xyes; then
  CFLAGS="$CFLAGS $OPENMP_CFLAGS"
  CXXFLAGS="$CXXFLAGS $OPENMP_CFLAGS"
fi

dnl
dnl check for libcrs
dnl
PKG_CHECK_MODULES([LIBCRS], [crs >= 2.0],
                  [ac_cv_libcrs_present=yes],
		  [ac_cv_libcrs_present=no])
if test $ac_cv_libcrs_present = yes; then
  CFLAGS="$CFLAGS $LIBCRS_CFLAGS"
  CXXFLAGS="$CXXFLAGS $LIBCRS_CFLAGS"
  LDFLAGS="$LDFLAGS $LIBCRS_LDFLAGS"
  LIBS="$LIBS $LIBCRS_LIBS"

  AC_SUBST([LIBCRS_CFLAGS])
  AC_SUBST([LIBCRS_LDFLAGS])
  AC_SUBST([LIBCRS_LIBS])
else
  AC_MSG_FAILURE([Can't find suitable version of libcrs installed. Please install one (https://github.com/o-peregudov/libcrs.git) and run configure once again])
fi

AC_CONFIG_FILES([
  Makefile
  mqmx/Makefile
  test/Makefile
  doc/Makefile
  doc/Doxyfile
])
AC_OUTPUT
