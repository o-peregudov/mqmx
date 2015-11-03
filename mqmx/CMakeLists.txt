SET (MQMX_SOURCES
  WaitTimeProvider.cpp
  MessageQueue.cpp
  message_queue_poll.cpp
)

SET (MQMX_HEADERS
  Types.h
  WaitTimeProvider.h
  Message.h
  MessageQueue.h
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
