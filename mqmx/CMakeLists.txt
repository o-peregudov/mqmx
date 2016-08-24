CONFIGURE_FILE (
  ${PROJECT_SOURCE_DIR}/mqmx/libexport.h.in
  ${PROJECT_BINARY_DIR}/mqmx/libexport.h
  @ONLY
  )

SET (MQMX_SOURCES
  message_queue.cpp
  message_queue_poll.cpp
  message_queue_pool.cpp
  wait_time_provider.cpp
  work_queue.cpp
)

SET (MQMX_HEADERS
  message.h
  message_queue.h
  message_queue_poll.h
  message_queue_pool.h
  types.h
  wait_time_provider.h
  work_queue.h
  ${PROJECT_BINARY_DIR}/mqmx/libexport.h
)

ADD_LIBRARY (${PROJECT_NAME}
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
  DESTINATION include/mqmx
)
