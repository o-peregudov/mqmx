SET (MQMX_SOURCES
  wait_time_provider.cpp
  message_queue.cpp
  message_queue_poll.cpp
  message_queue_pool.cpp
)

SET (MQMX_HEADERS
  types.h
  wait_time_provider.h
  message.h
  message_queue.h
  message_queue_poll.h
  message_queue_pool.h
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
