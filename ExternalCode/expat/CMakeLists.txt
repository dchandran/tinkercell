SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
#----------- include folders ----------------------

INCLUDE_DIRECTORIES( 
	BEFORE
	${TINKERCELL_SOURCE_DIR}/ExternalCode/expat
	${TINKERCELL_SOURCE_DIR}/ExternalCode/expat/lib
)

#------ DEFINITIONS --------------

IF (WIN32)
	ADD_DEFINITIONS(-DWIN32)
	ADD_DEFINITIONS(-D_WIN32)
	IF (NOT MINGW)
		ADD_DEFINITIONS(-Dstrcasecmp=_stricmp)
		ADD_DEFINITIONS(-Dstrncasecmp=_strnicmp)
	ENDIF(NOT MINGW)
ENDIF (WIN32)

ADD_DEFINITIONS(-DHAVE_EXPAT_CONFIG_H)
#------ EXPAT source files --------------

FILE( GLOB  EXPAT_SRC_FILES   ${TINKERCELL_SOURCE_DIR}/ExternalCode/expat/lib/*.c)

#------ make the library --------------

ADD_LIBRARY( expat
  #SHARED
  STATIC
  ${EXPAT_SRC_FILES}
)
