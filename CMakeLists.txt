CMAKE_MINIMUM_REQUIRED (VERSION 3.0)

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
INCLUDE (FindOpenMP)

############################################################
#
# Init output directories
#
############################################################

SET (EXECUTABLE_OUTPUT_PATH     ${CMAKE_BINARY_DIR}/bin)
SET (LIBRARY_OUTPUT_PATH        ${CMAKE_BINARY_DIR}/lib)
SET (INCLUDE_OUTPUT_PATH        ${CMAKE_BINARY_DIR}/include)

FIND_PACKAGE (Doxygen)
FIND_PACKAGE (Threads REQUIRED)
FIND_PACKAGE (GTest REQUIRED)

ADD_DEFINITIONS (-DHAVE_CONFIG_H)

#
# check if compilers supports -std=c++14 or -std=c++11
#
CHECK_CXX_COMPILER_FLAG (-std=c++14 STDCXX14_SUPPORTED)
IF (STDCXX14_SUPPORTED EQUAL 1)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
ELSE ()
  CHECK_CXX_COMPILER_FLAG (-std=c++11 STDCXX11_SUPPORTED)
  IF (STDCXX11_SUPPORTED EQUAL 1)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  ELSE ()
    MESSAGE (FATAL_ERROR "Support for 2011 ISO C++ or 2014 ISO C++ standard is required")
  ENDIF ()
ENDIF ()

#
# check if compilers supports -pedantic
#
CHECK_CXX_COMPILER_FLAG (-pedantic PEDANTIC_SUPPORTED)
IF (PEDANTIC_SUPPORTED EQUAL 1)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
ENDIF ()

#
# check if compilers supports -Wall
#
CHECK_CXX_COMPILER_FLAG (-Wall WALL_SUPPORTED)
IF (WALL_SUPPORTED EQUAL 1)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
ENDIF ()

#
# check if compilers supports -Werror
#
CHECK_CXX_COMPILER_FLAG (-Werror WERROR_SUPPORTED)
IF (WERROR_SUPPORTED EQUAL 1)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
ENDIF ()

#
# utilize OpenMP if found
#
IF (OPENMP_FOUND)
  SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ENDIF ()



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
