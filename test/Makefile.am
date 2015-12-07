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

<<<<<<< HEAD
MessageQueueSanity_SOURCES = MessageQueueSanity.cpp
MessageQueueListener_SOURCES = MessageQueueListener.cpp
MessageQueueListenerMoveCtor_SOURCES = MessageQueueListenerMoveCtor.cpp
MessageQueueListenerMoveAssignment_SOURCES = MessageQueueListenerMoveAssignment.cpp
MessageQueuePollSanity_SOURCES = MessageQueuePollSanity.cpp
MessageQueuePollInitialNotification_SOURCES = MessageQueuePollInitialNotification.cpp
MessageQueuePollInfiniteWait_SOURCES = MessageQueuePollInfiniteWait.cpp
MessageQueuePollRelTimeout_SOURCES = MessageQueuePollRelTimeout.cpp
MessageQueuePollAbsTimeout_SOURCES = MessageQueuePollAbsTimeout.cpp
=======
AM_DEFAULT_SOURCE_EXT = .cpp
>>>>>>> 512fc2f58a84f6f814fed36639ac6a73b475c135
