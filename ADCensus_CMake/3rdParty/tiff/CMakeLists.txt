CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(SRC
    tif_aux.c tif_close.c tif_codec.c tif_color.c tif_compress.c tif_dir.c tif_dirinfo.c tif_dirread.c tif_dirwrite.c tif_dumpmode.c tif_error.c tif_extension.c tif_fax3.c tif_fax3sm.c tif_flush.c tif_getimage.c tif_jbig.c tif_luv.c tif_lzw.c tif_next.c tif_open.c tif_packbits.c tif_pixarlog.c tif_predict.c tif_print.c tif_read.c tif_strip.c tif_swab.c tif_thunder.c tif_tile.c tif_unix.c tif_version.c tif_warning.c tif_write.c tif_zip.c
    t4.h tif_config.h tif_config.vc.h tif_dir.h tif_fax3.h tiff.h tiffconf.h tiffconf.vc.h tiffio.h tiffiop.h tiffvers.h tif_predict.h uvcode.h)

IF(NOT WITH_LOCAL_LIBTIFF)
    FIND_PACKAGE(TIFF)
ENDIF(NOT WITH_LOCAL_LIBTIFF)
IF(NOT TIFF_FOUND)
    IF(WIN32)
        ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)
    ENDIF(WIN32)
    IF(UNIX)
        INCLUDE(CheckTypeSize)
        CHECK_TYPE_SIZE(long SIZEOFLONG)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/tiffconf.gcc.h.cmakein
                       ${CMAKE_CURRENT_SOURCE_DIR}/tiffconf.gcc.h)
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/tif_config.gcc.h.cmakein
                       ${CMAKE_CURRENT_SOURCE_DIR}/tif_config.gcc.h)
    ENDIF(UNIX)
    ADD_LIBRARY(tiff ${SRC})
ENDIF(NOT TIFF_FOUND)

INSTALL(FILES CMakeLists.txt ${SRC}
              tiffconf.gcc.h.cmakein tif_config.gcc.h.cmakein
        DESTINATION src/3rdParty/tiff)
