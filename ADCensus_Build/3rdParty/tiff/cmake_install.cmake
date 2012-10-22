# Install script for directory: C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/src/3rdParty/tiff" TYPE FILE FILES
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/CMakeLists.txt"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_aux.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_close.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_codec.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_color.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_compress.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_dir.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_dirinfo.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_dirread.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_dirwrite.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_dumpmode.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_error.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_extension.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_fax3.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_fax3sm.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_flush.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_getimage.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_jbig.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_luv.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_lzw.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_next.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_open.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_packbits.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_pixarlog.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_predict.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_print.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_read.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_strip.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_swab.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_thunder.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_tile.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_unix.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_version.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_warning.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_write.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_zip.c"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/t4.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_config.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_config.vc.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_dir.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_fax3.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tiff.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tiffconf.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tiffconf.vc.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tiffio.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tiffiop.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tiffvers.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_predict.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/uvcode.h"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tiffconf.gcc.h.cmakein"
    "C:/Users/Yohann/Documents/GitHub/ADCensus/ADCensus_CMake/3rdParty/tiff/tif_config.gcc.h.cmakein"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

