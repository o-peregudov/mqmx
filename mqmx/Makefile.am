AM_CPPFLAGS = -I$(top_srcdir)
EXTRA_DIST = CMakeLists.txt

lib_LTLIBRARIES = libmqmx.la
libmqmx_la_LDFLAGS = -no-undefined -version-info $(MQMX_LT_VERSION)

pkginclude_HEADERS =		\
	types.h			\
	wait_time_provider.h	\
	message.h		\
	message_queue.h		\
	message_queue_poll.h    \
	message_queue_pool.h

nodist_pkginclude_HEADERS =

libmqmx_la_SOURCES =		\
	wait_time_provider.cpp	\
	message_queue.cpp	\
	message_queue_poll.cpp	\
	message_queue_pool.cpp

@CODE_COVERAGE_RULES@
