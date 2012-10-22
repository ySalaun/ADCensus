# Install script for directory: C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Stereo")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/3rdParty/zlib" TYPE FILE FILES
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/CMakeLists.txt"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/ChangeLog"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/INDEX"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/README"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/README.ipol"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/zconf.h.cmakein"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/zconf.h.in"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/zlib2ansi"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/zlib.3"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/zlib.map"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/zlib.pc.in"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/zconf.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/zlib.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/crc32.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/deflate.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/gzguts.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/inffast.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/inffixed.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/inflate.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/inftrees.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/trees.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/zutil.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/adler32.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/compress.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/crc32.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/deflate.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/gzclose.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/gzlib.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/gzread.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/gzwrite.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/inflate.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/infback.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/inftrees.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/inffast.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/trees.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/uncompr.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/zutil.c"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/3rdParty/zlib/win32" TYPE FILE FILES
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/win32/zlib1.rc"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/zlib/win32/zlib.def"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

