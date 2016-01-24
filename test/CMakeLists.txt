############################################################

ADD_EXECUTABLE (MessageQueueSanityChecks
  MessageQueueSanityChecks.cpp
)

TARGET_LINK_LIBRARIES (MessageQueueSanityChecks
  ${CMAKE_THREAD_LIBS_INIT}
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
  mqmx
)

ADD_TEST (NAME MessageQueue/Listener
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND MessageQueueListener
)

############################################################

ADD_EXECUTABLE (MessageQueueListenerMoveCtor
  MessageQueueListenerMoveCtor.cpp
)

TARGET_LINK_LIBRARIES (MessageQueueListenerMoveCtor
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Listener/Move-ctor
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND MessageQueueListenerMoveCtor
)

############################################################

ADD_EXECUTABLE (MessageQueueListenerMoveAssignment
  MessageQueueListenerMoveAssignment.cpp
)

TARGET_LINK_LIBRARIES (MessageQueueListenerMoveAssignment
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Listener/Move-assignment
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND MessageQueueListenerMoveAssignment
)

############################################################

ADD_EXECUTABLE (MessageQueuePollSanityChecks
  MessageQueuePollSanityChecks.cpp
)

TARGET_LINK_LIBRARIES (MessageQueuePollSanityChecks
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Poll/SanityChecks
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND MessageQueuePollSanityChecks
)

############################################################

ADD_EXECUTABLE (MessageQueuePollInitialNotification
  MessageQueuePollInitialNotification.cpp
)

TARGET_LINK_LIBRARIES (MessageQueuePollInitialNotification
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Poll/Initial-notification
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND MessageQueuePollInitialNotification
)

############################################################

ADD_EXECUTABLE (MessageQueuePollInfiniteWait
  MessageQueuePollInfiniteWait.cpp
)

TARGET_LINK_LIBRARIES (MessageQueuePollInfiniteWait
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Poll/Infinite-wait
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND MessageQueuePollInfiniteWait
)

############################################################

ADD_EXECUTABLE (MessageQueuePollRelTimeout
  MessageQueuePollRelTimeout.cpp
)

TARGET_LINK_LIBRARIES (MessageQueuePollRelTimeout
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Poll/Rel.-timeout
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND MessageQueuePollRelTimeout
)

############################################################

ADD_EXECUTABLE (MessageQueuePollAbsTimeout
  MessageQueuePollAbsTimeout.cpp
)

TARGET_LINK_LIBRARIES (MessageQueuePollAbsTimeout
  ${CMAKE_THREAD_LIBS_INIT}
  mqmx
)

ADD_TEST (NAME MessageQueue/Poll/Abs.-timeout
  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
  COMMAND MessageQueuePollAbsTimeout
)
