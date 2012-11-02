# Install script for directory: C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "C:/Program Files/Stereo")
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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/3rdParty/png" TYPE FILE FILES
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/CMakeLists.txt"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/LICENSE"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/README"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/README.ipol"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/png.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pnglibconf.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngconf.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngdebug.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pnginfo.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngpriv.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngstruct.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/png.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngerror.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngget.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngmem.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngpread.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngread.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngrio.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngrtran.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngrutil.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngset.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngtrans.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngwio.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngwrite.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngwtran.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/png/pngwutil.c"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

