ADD_LIBRARY (${PROJECT_NAME}_mocks STATIC
  fixtures/message_queue_poll.cpp
  fixtures/message_queue_poll.h
)

TARGET_LINK_LIBRARIES (${PROJECT_NAME}_mocks
  ${CMAKE_THREAD_LIBS_INIT}
  ${LIBCRS_LDFLAGS}
  ${PROJECT_NAME}
)

SET (LDADD
  ${CMAKE_THREAD_LIBS_INIT}
  ${LIBCRS_LDFLAGS}
  ${PROJECT_NAME}_mocks
  ${PROJECT_NAME}
)

SET (TESTS
  message_queue_listener_data_and_closed
  message_queue_listener_detached_because_of_move_assignment
  message_queue_listener_detached_because_of_move_ctor
  message_queue_poll_absolute_timeout
  message_queue_poll_infinite_wait
  message_queue_poll_initial_notifications
  message_queue_poll_listener
  message_queue_poll_relative_timeout
  message_queue_poll_sanity
  message_queue_pool
  message_queue_sanity
  work_queue_sanity
)

SET (check_PROGRAMS
  message_queue_listener_data_and_closed
  message_queue_listener_detached_because_of_move_assignment
  message_queue_listener_detached_because_of_move_ctor
  message_queue_poll_absolute_timeout
  message_queue_poll_infinite_wait
  message_queue_poll_initial_notifications
  message_queue_poll_listener
  message_queue_poll_relative_timeout
  message_queue_poll_sanity
  message_queue_pool
  message_queue_sanity
  work_queue_sanity
)

SET (AM_DEFAULT_SOURCE_EXT ".cpp")

FOREACH (CHECK_EXECUTABLE ${check_PROGRAMS})
  IF (DEFINED AM_DEFAULT_SOURCE_EXT)
    SET (CHECK_SOURCE_FILE_NAME "${CHECK_EXECUTABLE}${AM_DEFAULT_SOURCE_EXT}")
  ELSE ()
    SET (CHECK_SOURCE_FILE_NAME "${CHECK_EXECUTABLE}.c")
  ENDIF ()
  ADD_EXECUTABLE ("${CHECK_EXECUTABLE}" "${CHECK_SOURCE_FILE_NAME}")
  TARGET_LINK_LIBRARIES ("${CHECK_EXECUTABLE}" ${LDADD})
ENDFOREACH ()

FOREACH (TEST_EXECUTABLE ${TESTS})
  ADD_TEST (NAME "${TEST_EXECUTABLE}"
    WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
    COMMAND "${TEST_EXECUTABLE}"
  )
ENDFOREACH ()
