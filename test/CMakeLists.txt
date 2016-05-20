SET (${PROJECT_NAME}_MOCKS_AND_STUBS_HEADERS
  mocks/MessageQueueListener.h
  fixtures/MessageQueuePoll.h
)

SET (${PROJECT_NAME}_MOCKS_AND_STUBS_SOURCES
  mocks/MessageQueueListener.cpp
  fixtures/MessageQueuePoll.cpp
)

SET (MOCKS_AND_STUBS_LA
  ${PROJECT_NAME}MocksAndStubs
)

INCLUDE_DIRECTORIES (
  ${CMAKE_SOURCE_DIR}/test
)

ADD_LIBRARY (${MOCKS_AND_STUBS_LA} STATIC
  ${${PROJECT_NAME}_MOCKS_AND_STUBS_HEADERS}
  ${${PROJECT_NAME}_MOCKS_AND_STUBS_SOURCES}
)

TARGET_LINK_LIBRARIES (${MOCKS_AND_STUBS_LA}
  ${CMAKE_THREAD_LIBS_INIT}
  ${GMOCK_MAIN_LIBRARY}
  ${LIBCRS_LDFLAGS}
  mqmx
)

############################################################

ADD_EXECUTABLE (MessageQueueSanityChecks
  MessageQueueSanityChecks.cpp
)

TARGET_LINK_LIBRARIES (MessageQueueSanityChecks
  ${CMAKE_THREAD_LIBS_INIT}
  ${MOCKS_AND_STUBS_LA}
  ${GMOCK_MAIN_LIBRARY}
  ${LIBCRS_LDFLAGS}
  mqmx
)

ADD_TEST (NAME MessageQueue/SanityChecks
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND MessageQueueSanityChecks
)

############################################################

ADD_EXECUTABLE (MessageQueueListener
  MessageQueueListener.cpp
)

TARGET_LINK_LIBRARIES (MessageQueueListener
  ${CMAKE_THREAD_LIBS_INIT}
  ${MOCKS_AND_STUBS_LA}
  ${GMOCK_MAIN_LIBRARY}
  ${LIBCRS_LDFLAGS}
  mqmx
)

ADD_TEST (NAME MessageQueue/Listener
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND MessageQueueListener
)

############################################################

ADD_EXECUTABLE (MessageQueuePoll
  MessageQueuePoll.cpp
)

TARGET_LINK_LIBRARIES (MessageQueuePoll
  ${CMAKE_THREAD_LIBS_INIT}
  ${MOCKS_AND_STUBS_LA}
  ${GMOCK_MAIN_LIBRARY}
  ${LIBCRS_LDFLAGS}
  mqmx
)

ADD_TEST (NAME MessageQueue/Poll
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND MessageQueuePoll
)

############################################################

ADD_EXECUTABLE (MessageQueuePool
  MessageQueuePool.cpp
)

TARGET_LINK_LIBRARIES (MessageQueuePool
  ${CMAKE_THREAD_LIBS_INIT}
  ${MOCKS_AND_STUBS_LA}
  ${GMOCK_MAIN_LIBRARY}
  ${LIBCRS_LDFLAGS}
  mqmx
)

ADD_TEST (NAME MessageQueue/Pool
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND MessageQueuePool
)
