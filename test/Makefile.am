ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir)
LDADD = $(top_builddir)/mqmx/libmqmx.la

AM_TESTS_ENVIRONMENT = LD_LIBRARY_PATH=$(top_builddir)/test/.libs:$(top_builddir)/test:$$LD_LIBRARY_PATH; export LD_LIBRARY_PATH;

TESTS =						\
	MessageQueueSanity			\
	MessageQueueListener			\
	MessageQueueListenerMoveCtor		\
	MessageQueueListenerMoveAssignment	\
	MessageQueuePollSanity			\
	MessageQueuePollInitialNotification	\
	MessageQueuePollInfiniteWait		\
	MessageQueuePollRelTimeout		\
	MessageQueuePollAbsTimeout

check_PROGRAMS =				\
	MessageQueueSanity			\
	MessageQueueListener			\
	MessageQueueListenerMoveCtor		\
	MessageQueueListenerMoveAssignment	\
	MessageQueuePollSanity			\
	MessageQueuePollInitialNotification	\
	MessageQueuePollInfiniteWait		\
	MessageQueuePollRelTimeout		\
	MessageQueuePollAbsTimeout

AM_DEFAULT_SOURCE_EXT = .cpp
