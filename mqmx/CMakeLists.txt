SET (MQMX_SOURCES
  wait_time_provider.cpp
  message_queue.cpp
  message_queue_poll.cpp
  MessageQueuePool.cpp
)

SET (MQMX_HEADERS
  types.h
  message.h
  message_queue.h
  wait_time_provider.h
  message_queue_poll.h
  MessageQueuePool.h
)

ADD_LIBRARY (${PROJECT_NAME} STATIC
  ${MQMX_COMMON_HEADERS_NODIST}
  ${MQMX_SOURCES}
  ${MQMX_HEADERS}
)

TARGET_LINK_LIBRARIES (${PROJECT_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
  ${LIBCRS_LDFLAGS}
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
