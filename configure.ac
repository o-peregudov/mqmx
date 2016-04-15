AC_INIT([mqmx], [0.0.0], [oleg.peregudov@nokia.com])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
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
dnl Check for Google Test/Mock frameworks
dnl
HAVE_LIBGTEST=0
HAVE_LIBGMOCK=0
AC_LANG_PUSH([C++])
AC_CHECK_HEADER(gtest/gtest.h, [ac_gtest_h_found=yes], [ac_gtest_h_found=no])
if test $ac_gtest_h_found = yes; then
  save_LIBS="$LIBS"
  LIBS="$LIBS -lgtest -lgtest_main"
  AC_CACHE_CHECK(for gtest library, ac_cv_libgtest_works,
    AC_LINK_IFELSE([AC_LANG_SOURCE([[
      #include <gtest/gtest.h>
      int main (int argc, char ** argv)
      {
        ::testing::InitGoogleTest(&argc, argv);
        return 0;
      }
    ]])],[ac_cv_libgtest_works=yes],[ac_cv_libgtest_works=no]))
  LIBS="$save_LIBS"
  if test $ac_cv_libgtest_works = yes; then
    LIBS="$LIBS -lgtest -lgtest_main"
    HAVE_LIBGTEST=1
  else
    AC_MSG_FAILURE([failed to link with libgtest])
  fi

  AC_CHECK_HEADER(gmock/gmock.h,
    [ac_gmock_h_found=yes], [ac_gmock_h_found=no],
    [#include <gtest/gtest.h>])
  if test $ac_gmock_h_found = yes; then
    save_LIBS="$LIBS"
    LIBS="$LIBS -lgmock -lgmock_main"
    AC_CACHE_CHECK(for gmock library, ac_cv_libgmock_works,
      AC_LINK_IFELSE([AC_LANG_SOURCE([[
        #include <gmock/gmock.h>
        int main (int argc, char ** argv)
        {
          ::testing::InitGoogleMock(&argc, argv);
          return 0;
        }
      ]])],[ac_cv_libgmock_works=yes],[ac_cv_libgmock_works=no]))
    LIBS="$save_LIBS"
    if test $ac_cv_libgmock_works = yes; then
      LIBS="$LIBS -lgmock -lgmock_main"
      HAVE_LIBGMOCK=1
    else
      AC_MSG_FAILURE([failed to link with libgmock])
    fi
  else
    AC_MSG_FAILURE([gmock.h is mandatory])
  fi
else
  AC_MSG_FAILURE([gtest.h is mandatory])
fi
AC_LANG_POP([C++])
AC_SUBST(HAVE_LIBGTEST)
AC_SUBST(HAVE_LIBGMOCK)

DX_INIT_DOXYGEN($PACKAGE_NAME)

AC_CONFIG_FILES([
    Makefile
    mqmx/Makefile
    test/Makefile
])
AC_OUTPUT
