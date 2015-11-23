CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (mqmx C CXX)
ENABLE_TESTING ()

############################################################
#
# CMake function(s) and macro(s)
#
############################################################

INCLUDE (CPack)
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

FIND_PACKAGE (Threads REQUIRED)
FIND_PACKAGE (Doxygen)

ADD_DEFINITIONS (-DHAVE_CONFIG_H)

#
# check if compilers supports -std=c++14 or -std=c++11
#
CHECK_CXX_COMPILER_FLAG (-std=c++14 SUPPORTS_STDCXX14)
IF (SUPPORTS_STDCXX14 EQUAL 1)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
ELSE ()
  CHECK_CXX_COMPILER_FLAG (-std=c++11 SUPPORTS_STDCXX11)
  IF (SUPPORTS_STDCXX11 EQUAL 1)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  ELSE ()
    MESSAGE (FATAL_ERROR "Support for 2011 ISO C++ or 2014 ISO C++ standard is required")
  ENDIF ()
ENDIF ()

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

INCLUDE_DIRECTORIES (
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${INCLUDE_OUTPUT_PATH}
)

LINK_DIRECTORIES (
  ${LIBRARY_OUTPUT_PATH}
  ${CMAKE_INSTALL_PREFIX}/lib
)

ADD_SUBDIRECTORY (Common)
ADD_SUBDIRECTORY (mqmx)
ADD_SUBDIRECTORY (test)
