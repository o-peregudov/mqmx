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
INCLUDE (FindPkgConfig)

############################################################
#
# Init output directories
#
############################################################

SET (EXECUTABLE_OUTPUT_PATH     ${CMAKE_BINARY_DIR}/bin)
SET (LIBRARY_OUTPUT_PATH        ${CMAKE_BINARY_DIR}/lib)
SET (INCLUDE_OUTPUT_PATH        ${CMAKE_BINARY_DIR}/include)

SET (CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_SOURCE_DIR}
)

FIND_PACKAGE (Threads REQUIRED)
FIND_PACKAGE (GTest REQUIRED)
FIND_PACKAGE (GMock REQUIRED)
FIND_PACKAGE (Doxygen)

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
INCLUDE (FindOpenMP)
IF (OPENMP_FOUND)
  SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ENDIF ()

#
# check for libcrs
#
PKG_CHECK_MODULES (LIBCRS REQUIRED crs>=2.0)
IF (LIBCRS_FOUND)
  SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBCRS_CFLAGS}")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBCRS_CFLAGS}")
ELSE ()
  MESSAGE (FATAL_ERROR "Can't find suitable version of libcrs installed. Please install one (https://github.com/o-peregudov/libcrs.git) and run cmake configuration once again")
ENDIF ()

INCLUDE_DIRECTORIES (
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${INCLUDE_OUTPUT_PATH}
  ${LIBCRS_INCLUDEDIR}
  ${GTEST_INCLUDE_DIRS}
  ${GMOCK_INCLUDE_DIRS}
)

LINK_DIRECTORIES (
  ${LIBRARY_OUTPUT_PATH}
  ${CMAKE_INSTALL_PREFIX}/lib
)

ADD_SUBDIRECTORY (mqmx)
ADD_SUBDIRECTORY (test)
