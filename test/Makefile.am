ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS =	-I$(top_builddir)	\
		-I$(top_srcdir)

check_LTLIBRARIES = libmqmx_mocks.la
libmqmx_mocks_la_SOURCES = 			\
	mocks/MessageQueueListener.h		\
	mocks/MessageQueueListener.cpp		\
	fixtures/MessageQueuePoll.h		\
	fixtures/MessageQueuePoll.cpp

libmqmx_mocks_la_LIBADD = $(top_builddir)/mqmx/libmqmx.la
libmqmx_mocks_la_LDFLAGS = -avoid-version -no-undefined -module -rpath /nowhere

LDADD = $(top_builddir)/mqmx/libmqmx.la libmqmx_mocks.la

AM_TESTS_ENVIRONMENT = LD_LIBRARY_PATH=$(top_builddir)/test/.libs:$(top_builddir)/test:$$LD_LIBRARY_PATH; export LD_LIBRARY_PATH;

TESTS =	message_queue_sanity	\
	message_queue_listener	\
	message_queue_poll	\
	MessageQueuePool

check_PROGRAMS =		\
	message_queue_sanity	\
	message_queue_listener	\
	message_queue_poll	\
	MessageQueuePool

AM_DEFAULT_SOURCE_EXT = .cpp

@CODE_COVERAGE_RULES@
