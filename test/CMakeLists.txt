############################################################

ADD_EXECUTABLE (message_queue_sanity
  message_queue_sanity.cpp
)

TARGET_LINK_LIBRARIES (message_queue_sanity
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Sanity
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND message_queue_sanity
)

############################################################

ADD_EXECUTABLE (message_queue_listener_move_ctor
  message_queue_listener_move_ctor.cpp
)

TARGET_LINK_LIBRARIES (message_queue_listener_move_ctor
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Listener/Move-ctor
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND message_queue_listener_move_ctor
)

############################################################

ADD_EXECUTABLE (message_queue_listener_move_assignment
  message_queue_listener_move_assignment.cpp
)

TARGET_LINK_LIBRARIES (message_queue_listener_move_assignment
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Listener/Move-assignment
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND message_queue_listener_move_assignment
)

############################################################

ADD_EXECUTABLE (message_queue_poll_sanity
  message_queue_poll_sanity.cpp
)

TARGET_LINK_LIBRARIES (message_queue_poll_sanity
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Poll/Sanity
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND message_queue_poll_sanity
)

############################################################

ADD_EXECUTABLE (message_queue_poll_initial_notification
  message_queue_poll_initial_notification.cpp
)

TARGET_LINK_LIBRARIES (message_queue_poll_initial_notification
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Poll/Initial-notification
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND message_queue_poll_initial_notification
)

############################################################

ADD_EXECUTABLE (message_queue_poll_infinite_wait
  message_queue_poll_infinite_wait.cpp
)

TARGET_LINK_LIBRARIES (message_queue_poll_infinite_wait
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Poll/Infinite-wait
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND message_queue_poll_infinite_wait
)

############################################################

ADD_EXECUTABLE (message_queue_poll_rel_timeout
  message_queue_poll_rel_timeout.cpp
)

TARGET_LINK_LIBRARIES (message_queue_poll_rel_timeout
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Poll/Rel.-timeout
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND message_queue_poll_rel_timeout
)

############################################################

ADD_EXECUTABLE (message_queue_poll_abs_timeout
  message_queue_poll_abs_timeout.cpp
)

TARGET_LINK_LIBRARIES (message_queue_poll_abs_timeout
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Poll/Abs.-timeout
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND message_queue_poll_abs_timeout
)
