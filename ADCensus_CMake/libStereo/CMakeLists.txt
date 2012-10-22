CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(libStereo)

SET(SRC patch.cpp patch.h)

ADD_LIBRARY(Stereo ${SRC})

IF(UNIX)
    SET_TARGET_PROPERTIES(Stereo PROPERTIES
                          COMPILE_FLAGS "-Wall -Wextra -Werror")
ENDIF(UNIX)

INCLUDE_DIRECTORIES(..)

INSTALL(FILES CMakeLists.txt ${SRC} DESTINATION src/libStereo)
