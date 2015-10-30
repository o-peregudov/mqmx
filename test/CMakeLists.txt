ENABLE_TESTING ()

INCLUDE_DIRECTORIES (
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
)

LINK_DIRECTORIES (
  ${LIBRARY_OUTPUT_PATH}
)

############################################################

ADD_EXECUTABLE (message_queue_sanity
  message_queue_sanity.cpp
)

TARGET_LINK_LIBRARIES (message_queue_sanity
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME message_queue_sanity
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND message_queue_sanity
)

############################################################

ADD_EXECUTABLE (message_queue_listener
  message_queue_listener.cpp
)

TARGET_LINK_LIBRARIES (message_queue_listener
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME message_queue_listener
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND message_queue_listener
)

############################################################

ADD_EXECUTABLE (message_queue_poll
  message_queue_poll.cpp
)

TARGET_LINK_LIBRARIES (message_queue_poll
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME message_queue_poll
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND message_queue_poll
)
