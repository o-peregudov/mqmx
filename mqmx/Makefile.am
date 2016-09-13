AM_CPPFLAGS = -I$(top_srcdir)
nodist_pkginclude_HEADERS =

lib_LTLIBRARIES = libmqmx.la
libmqmx_la_LDFLAGS = -no-undefined -version-info $(MQMX_LT_VERSION)

pkginclude_HEADERS =
pkginclude_HEADERS += libexport.h
pkginclude_HEADERS += message.h
pkginclude_HEADERS += message_queue.h
pkginclude_HEADERS += message_queue_poll.h
pkginclude_HEADERS += message_queue_pool.h
pkginclude_HEADERS += types.h
pkginclude_HEADERS += wait_time_provider.h
pkginclude_HEADERS += work_queue.h

pkginclude_testingdir = $(pkgincludedir)/testing
pkginclude_testing_HEADERS =
pkginclude_testing_HEADERS += testing/work_queue_for_tests.h

libmqmx_la_SOURCES =
libmqmx_la_SOURCES += message_queue.cpp
libmqmx_la_SOURCES += message_queue_poll.cpp
libmqmx_la_SOURCES += message_queue_pool.cpp
libmqmx_la_SOURCES += wait_time_provider.cpp
libmqmx_la_SOURCES += work_queue.cpp
libmqmx_la_SOURCES += testing/work_queue_for_tests.cpp

@CODE_COVERAGE_RULES@

EXTRA_DIST = CMakeLists.txt
