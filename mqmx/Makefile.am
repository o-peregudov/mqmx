AM_CPPFLAGS = -I$(top_srcdir)
EXTRA_DIST = CMakeLists.txt

lib_LTLIBRARIES = libmqmx.la
libmqmx_la_LDFLAGS = -no-undefined -version-info $(MQMX_LT_VERSION)

pkginclude_HEADERS =		\
	Types.h			\
	WaitTimeProvider.h	\
	Message.h		\
	MessageQueue.h		\
	MessageQueuePoll.h      \
	MessageQueuePool.h

nodist_pkginclude_HEADERS =

libmqmx_la_SOURCES =		\
	WaitTimeProvider.cpp	\
	MessageQueue.cpp	\
	MessageQueuePoll.cpp	\
	MessageQueuePool.cpp

@CODE_COVERAGE_RULES@
