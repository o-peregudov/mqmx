CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (mqmx C CXX)
ENABLE_TESTING ()

############################################################
#
# CMake function(s) and macro(s)
#
############################################################

INCLUDE (InstallRequiredSystemLibraries)
INCLUDE (CheckCXXSourceRuns)
INCLUDE (CheckCXXSourceCompiles)
INCLUDE (CheckCCompilerFlag)
INCLUDE (CheckCXXCompilerFlag)
INCLUDE (CheckFunctionExists)
INCLUDE (CheckIncludeFileCXX)
INCLUDE (CheckLibraryExists)
INCLUDE (CheckIncludeFile)
INCLUDE (ExternalProject)

############################################################
#
# Init output directories
#
############################################################

SET (EXECUTABLE_OUTPUT_PATH     ${CMAKE_BINARY_DIR}/bin)
SET (LIBRARY_OUTPUT_PATH        ${CMAKE_BINARY_DIR}/lib)
SET (INCLUDE_OUTPUT_PATH        ${CMAKE_BINARY_DIR}/include)
SET (DOC_OUTPUT_PATH            ${CMAKE_BINARY_DIR}/doc)

FIND_PACKAGE (Threads)
FIND_PACKAGE (Doxygen)

ADD_DEFINITIONS (-DHAVE_CONFIG_H)

#
# check if compilers supports -std=c++11
#
CHECK_CXX_COMPILER_FLAG (-std=c++11 SUPPORTS_STDCXX11)
IF (SUPPORTS_STDCXX11 EQUAL 1)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ENDIF (SUPPORTS_STDCXX11 EQUAL 1)

#
# check if compilers supports -pedantic
#
CHECK_CXX_COMPILER_FLAG (-pedantic SUPPORTS_PEDANTIC)
IF (SUPPORTS_PEDANTIC EQUAL 1)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
ENDIF (SUPPORTS_PEDANTIC EQUAL 1)

#
# check if compilers supports -Wall
#
CHECK_CXX_COMPILER_FLAG (-pedantic SUPPORTS_WALL)
IF (SUPPORTS_WALL EQUAL 1)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
ENDIF (SUPPORTS_WALL EQUAL 1)

#
# check if compilers supports -Werror
#
CHECK_CXX_COMPILER_FLAG (-Werror SUPPORTS_WERROR)
IF (SUPPORTS_WERROR EQUAL 1)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
ENDIF (SUPPORTS_WERROR EQUAL 1)

#
# [C++ 11] auto-detect and verify that <mutex> is supported
#
CHECK_INCLUDE_FILE_CXX (mutex HAVE_MUTEX)
CHECK_CXX_SOURCE_COMPILES("
#include <mutex>
int main (int argc, const char ** argv)
{
  std::mutex mtx;
  mtx.lock ();
  if (mtx.try_lock ())
    return 1;
  mtx.unlock ();
  return 0;
}
" HAVE_CXX_MUTEX)

IF (NOT "${HAVE_CXX_MUTEX}" STREQUAL "1")
  MESSAGE (FATAL_ERROR "C++ 11 <mutex> is required")
ENDIF ()

INCLUDE_DIRECTORIES (
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${INCLUDE_OUTPUT_PATH}
)

LINK_DIRECTORIES (
  ${LIBRARY_OUTPUT_PATH}
  ${CMAKE_INSTALL_PREFIX}/lib
)

SET (MQMX_SOURCES
  wait_time_provider.cpp
  message_queue.cpp
  message_queue_poll.cpp
)

SET (MQMX_HEADERS
  types.h
  wait_time_provider.h
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
