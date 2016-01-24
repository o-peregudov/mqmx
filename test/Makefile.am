ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS =	-I$(top_builddir)	\
		-I$(top_srcdir)

LDADD = $(top_builddir)/mqmx/libmqmx.la

AM_TESTS_ENVIRONMENT = LD_LIBRARY_PATH=$(top_builddir)/test/.libs:$(top_builddir)/test:$$LD_LIBRARY_PATH; export LD_LIBRARY_PATH;

noinst_HEADERS =	  			\
	stubs/listener.h			\
	fixtures/MessageQueuePoll.h

TESTS =	MessageQueueSanityChecks		\
	MessageQueueListener			\
	MessageQueueListenerMoveCtor		\
	MessageQueueListenerMoveAssignment	\
	MessageQueuePollSanityChecks		\
	MessageQueuePollInitialNotification	\
	MessageQueuePollInfiniteWait		\
	MessageQueuePollRelTimeout		\
	MessageQueuePollAbsTimeout

check_PROGRAMS =				\
	MessageQueueSanityChecks		\
	MessageQueueListener			\
	MessageQueueListenerMoveCtor		\
	MessageQueueListenerMoveAssignment	\
	MessageQueuePollSanityChecks		\
	MessageQueuePollInitialNotification	\
	MessageQueuePollInfiniteWait		\
	MessageQueuePollRelTimeout		\
	MessageQueuePollAbsTimeout

AM_DEFAULT_SOURCE_EXT = .cpp
