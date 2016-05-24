INCLUDE_DIRECTORIES (
  ${CMAKE_SOURCE_DIR}/test
)

ADD_LIBRARY (${PROJECT_NAME}_mocks STATIC
  mocks/message_queue_listener.h
  mocks/message_queue_listener.cpp
  fixtures/MessageQueuePoll.h
  fixtures/MessageQueuePoll.cpp
)

TARGET_LINK_LIBRARIES (${PROJECT_NAME}_mocks
  ${CMAKE_THREAD_LIBS_INIT}
  ${GMOCK_MAIN_LIBRARY}
  ${LIBCRS_LDFLAGS}
  ${PROJECT_NAME}
)

SET (LDADD
  ${CMAKE_THREAD_LIBS_INIT}
  ${GMOCK_MAIN_LIBRARY}
  ${LIBCRS_LDFLAGS}
  ${PROJECT_NAME}_mocks
  ${PROJECT_NAME}
)

SET (TESTS
  message_queue_sanity
  message_queue_listener
  message_queue_poll
  message_queue_pool
)

SET (check_PROGRAMS
  message_queue_sanity
  message_queue_listener
  message_queue_poll
  message_queue_pool
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
