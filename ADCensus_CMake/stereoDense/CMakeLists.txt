CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(stereoDense)

SET(SRC
	stereoDense.cpp parameters.cpp parameters.h)

ADD_EXECUTABLE(stereoDense ${SRC})

INCLUDE_DIRECTORIES(..)

FIND_PACKAGE(OpenMP)
IF(OPENMP_FOUND)
    SET_TARGET_PROPERTIES(stereoDense PROPERTIES
                          COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
ENDIF(OPENMP_FOUND)

IF(UNIX)
    SET_TARGET_PROPERTIES(stereoDense PROPERTIES
                          COMPILE_FLAGS "-Wall -Wextra -Werror ${OpenMP_CXX_FLAGS}")
    IF(OPENMP_FOUND)
        SET(CMAKE_EXE_LINKER_FLAGS ${OpenMP_CXX_FLAGS})
    ENDIF(OPENMP_FOUND)
ENDIF(UNIX)

TARGET_LINK_LIBRARIES(stereoDense Stereo IO)

INSTALL(FILES CMakeLists.txt ${SRC} DESTINATION src/stereoDense)
