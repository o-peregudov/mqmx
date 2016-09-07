AM_CPPFLAGS =
AM_CPPFLAGS += -I$(top_builddir)
AM_CPPFLAGS += -I$(top_srcdir)

check_LTLIBRARIES =
check_LTLIBRARIES += libmqmx_mocks.la
libmqmx_mocks_la_SOURCES =
libmqmx_mocks_la_SOURCES += fixtures/message_queue_poll.cpp
libmqmx_mocks_la_SOURCES += fixtures/message_queue_poll.h
libmqmx_mocks_la_SOURCES += helpers/work_queue_for_tests.cpp
libmqmx_mocks_la_SOURCES += helpers/work_queue_for_tests.h

libmqmx_mocks_la_LIBADD = $(top_builddir)/mqmx/libmqmx.la
libmqmx_mocks_la_LDFLAGS = -avoid-version -no-undefined -rpath /nowhere

LDADD =
LDADD += $(top_builddir)/mqmx/libmqmx.la
LDADD += libmqmx_mocks.la

AM_TESTS_ENVIRONMENT = LD_LIBRARY_PATH=$(top_builddir)/test/.libs:$(top_builddir)/test:$$LD_LIBRARY_PATH; export LD_LIBRARY_PATH;

TESTS =
TESTS += message_queue_listener_data_and_closed
TESTS += message_queue_listener_detached_because_of_move_assignment
TESTS += message_queue_listener_detached_because_of_move_ctor
TESTS += message_queue_poll_absolute_timeout
TESTS += message_queue_poll_infinite_wait
TESTS += message_queue_poll_initial_notifications
TESTS += message_queue_poll_listener
TESTS += message_queue_poll_relative_timeout
TESTS += message_queue_poll_sanity
TESTS += message_queue_pool
TESTS += message_queue_sanity
TESTS += work_queue_cancel_work
TESTS += work_queue_for_tests_cancel_work
TESTS += work_queue_for_tests_schedule_work
TESTS += work_queue_sanity
TESTS += work_queue_schedule_work
TESTS += work_queue_schedule_work_periodic
TESTS += work_queue_update_work

check_PROGRAMS =
check_PROGRAMS += message_queue_listener_data_and_closed
check_PROGRAMS += message_queue_listener_detached_because_of_move_assignment
check_PROGRAMS += message_queue_listener_detached_because_of_move_ctor
check_PROGRAMS += message_queue_poll_absolute_timeout
check_PROGRAMS += message_queue_poll_infinite_wait
check_PROGRAMS += message_queue_poll_initial_notifications
check_PROGRAMS += message_queue_poll_listener
check_PROGRAMS += message_queue_poll_relative_timeout
check_PROGRAMS += message_queue_poll_sanity
check_PROGRAMS += message_queue_pool
check_PROGRAMS += message_queue_sanity
check_PROGRAMS += work_queue_cancel_work
check_PROGRAMS += work_queue_for_tests_cancel_work
check_PROGRAMS += work_queue_for_tests_schedule_work
check_PROGRAMS += work_queue_sanity
check_PROGRAMS += work_queue_schedule_work
check_PROGRAMS += work_queue_schedule_work_periodic
check_PROGRAMS += work_queue_update_work

AM_DEFAULT_SOURCE_EXT = .cpp

@CODE_COVERAGE_RULES@

EXTRA_DIST = FakeIt CMakeLists.txt
dist-hook:
	rm -f $(distdir)/FakeIt/.git
	rm -f $(distdir)/FakeIt/.gitattributes
	rm -f $(distdir)/FakeIt/.gitignore
	rm -f $(distdir)/FakeIt/.gitmodules
