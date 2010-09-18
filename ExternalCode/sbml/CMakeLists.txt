SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

IF (WIN32)

	IF (MINGW)
		SET (LIBXML2_LIBRARIES ${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/win32/libxml2/lib/libxml2.dll.a)
	ELSE(MINGW)
		SET (LIBXML2_LIBRARIES ${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/win32/libxml2/lib/libxml2.lib)
	ENDIF(MINGW)
	SET (LIBXML2_INCLUDE_DIR ${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/win32/libxml2/include)
	
ELSE(WIN32)	

	FIND_PACKAGE(LibXml2)
	
ENDIF(WIN32)

INCLUDE_DIRECTORIES( 
	BEFORE
	${LIBXML2_INCLUDE_DIR}
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/xml 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/util 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/validator 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/validator/constraints/
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/units 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/sbml
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/sbml/layout 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/math 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/common 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/compress 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/annotation
)

#ADD_DEFINITIONS(-DLIBSBML_STATIC)
ADD_DEFINITIONS(-DLIBSBML_EXPORTS)
ADD_DEFINITIONS(-DLIBLAX_EXPORTS)

ADD_DEFINITIONS(-DPACKAGE_NAME=\"libSBML\")
ADD_DEFINITIONS(-DNDEBUG)
ADD_DEFINITIONS(-DUSE_LIBXML)
ADD_DEFINITIONS(-DUSE_LAYOUT)

IF (WIN32)
	ADD_DEFINITIONS(-DWIN32)
	ADD_DEFINITIONS(-D_WIN32)
	ADD_DEFINITIONS(-D_WINDOWS)
	ADD_DEFINITIONS(-D_USRDLL)
	ADD_DEFINITIONS(-D_EXPORTS)
	ADD_DEFINITIONS(-DIN_LIBXML)
	IF (MINGW)
		ADD_DEFINITIONS(-DCYGWIN)
	ENDIF(MINGW)
ENDIF (WIN32)

FILE( GLOB SBML_SRC_FILES
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/xml/XML*.cpp 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/xml/XML*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/xml/LibXML*.cpp 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/xml/LibXML*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/util/*.cpp 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/util/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/validator/*.cpp 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/validator/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/validator/constraints/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/validator/constraints/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/units/*.cpp 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/units/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/math/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/math/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/annotation/*.cpp 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/annotation/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/sbml/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/sbml/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/sbml/layout/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/sbml/layout/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/common/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/common/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/compress/CompressCommon.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/compress/InputDecompressor.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/compress/OutputCompressor.cpp
)

ADD_LIBRARY( sbml
  SHARED
  #STATIC
  ${SBML_SRC_FILES}
)

TARGET_LINK_LIBRARIES( sbml
  sbml
  ${LIBXML2_LIBRARIES}
)


IF ( WIN32 )
   FILE( GLOB LIBXML_LIB_FILES ${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/win32/libxml2/lib/*.dll DESTINATION bin/../ )
   INSTALL(FILES ${LIBXML_LIB_FILES} DESTINATION bin/../)
   INSTALL(TARGETS sbml DESTINATION bin/../)
ELSE ( WIN32 )
   INSTALL(TARGETS sbml LIBRARY DESTINATION bin/../)
ENDIF( WIN32 )

