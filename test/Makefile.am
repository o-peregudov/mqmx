ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS =	-I$(top_builddir)	\
		-I$(top_srcdir)

check_LTLIBRARIES = libmqmx_mocks.la
libmqmx_mocks_la_SOURCES = 			\
	mocks/MessageQueueListener.h		\
	mocks/MessageQueueListener.cpp

libmqmx_mocks_la_LIBADD = $(top_builddir)/mqmx/libmqmx.la
libmqmx_mocks_la_LDFLAGS = -avoid-version -no-undefined -module -rpath /nowhere

LDADD = $(top_builddir)/mqmx/libmqmx.la libmqmx_mocks.la

AM_TESTS_ENVIRONMENT = LD_LIBRARY_PATH=$(top_builddir)/test/.libs:$(top_builddir)/test:$$LD_LIBRARY_PATH; export LD_LIBRARY_PATH;

noinst_HEADERS =	  			\
	fixtures/MessageQueuePoll.h

TESTS =	MessageQueueSanityChecks		\
	MessageQueueListener			\
	MessageQueuePollSanityChecks		\
	MessageQueuePollInitialNotification	\
	MessageQueuePollInfiniteWait		\
	MessageQueuePollRelTimeout		\
	MessageQueuePollAbsTimeout

check_PROGRAMS =				\
	MessageQueueSanityChecks		\
	MessageQueueListener			\
	MessageQueuePollSanityChecks		\
	MessageQueuePollInitialNotification	\
	MessageQueuePollInfiniteWait		\
	MessageQueuePollRelTimeout		\
	MessageQueuePollAbsTimeout

AM_DEFAULT_SOURCE_EXT = .cpp

@CODE_COVERAGE_RULES@
