SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
#----------- include folders ----------------------

INCLUDE_DIRECTORIES( 
	BEFORE
	${TINKERCELL_SOURCE_DIR}/ExternalCode/expat
	${TINKERCELL_SOURCE_DIR}/ExternalCode/expat/lib
)

#------ DEFINITIONS --------------

ADD_DEFINITIONS(-DHAVE_EXPAT_CONFIG_H)

IF (WIN32)
	ADD_DEFINITIONS(-DWIN32)
	ADD_DEFINITIONS(-D_WIN32)
ENDIF (WIN32)

#------ SBML source files --------------

FILE( GLOB     EXPAT_SRC_FILES   ${TINKERCELL_SOURCE_DIR}/ExternalCode/expat/lib/*.c)

#------ make the library --------------

ADD_LIBRARY( expat
  #SHARED
  STATIC
  ${EXPAT_SRC_FILES}
)

#------ install the library in the main TinkerCell folder --------------

#IF ( WIN32 )
#   INSTALL(TARGETS expat DESTINATION bin/../)
#ELSE ( WIN32 )
#   INSTALL(TARGETS expat LIBRARY DESTINATION bin/../)
#ENDIF( WIN32 )

