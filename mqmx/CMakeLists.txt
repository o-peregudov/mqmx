SET (MQMX_SOURCES
  WaitTimeProvider.cpp
  message_queue.cpp
  message_queue_poll.cpp
)

SET (MQMX_HEADERS
  types.h
  WaitTimeProvider.h
  message.h
  message_queue.h
  message_queue_poll.h
)

ADD_LIBRARY (${PROJECT_NAME} STATIC
  ${MQMX_SOURCES}
  ${MQMX_HEADERS}
)

TARGET_LINK_LIBRARIES (${PROJECT_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
)

INSTALL (
  TARGETS ${PROJECT_NAME}
  DESTINATION lib
)

INSTALL (
  FILES ${MQMX_HEADERS}
  DESTINATION mqmx
)

INSTALL (
  FILES ${MQMX_SOURCES}
  DESTINATION mqmx
)
