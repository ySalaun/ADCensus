#---------------------
#Third party libraries
#---------------------

ADD_SUBDIRECTORY(zlib)
ADD_SUBDIRECTORY(png)
ADD_SUBDIRECTORY(tiff)

INSTALL(FILES CMakeLists.txt DESTINATION src/3rdParty)
