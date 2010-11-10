SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

IF (WIN32)

	IF (MINGW)
		SET (LIBXML2_LIBRARIES ${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/win32/libxml2/lib/libxml2.dll.a)
	ELSE(MINGW)
		SET (LIBXML2_LIBRARIES ${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/win32/libxml2/lib/libxml2.lib)
	ENDIF(MINGW)
	SET (LIBXML2_INCLUDE_DIR ${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/win32/libxml2/include)
	
ELSE(WIN32)	

	FIND_PACKAGE(LibXml2)
	
ENDIF(WIN32)

INCLUDE_DIRECTORIES( 
	BEFORE
	${LIBXML2_INCLUDE_DIR}
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/include
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/xml 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/util 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/validator 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/validator/constraints/
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/units 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/sbml
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/sbml/layout 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/math 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/common 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/compress 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/annotation
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
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/xml/XML*.cpp 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/xml/XML*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/xml/LibXML*.cpp 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/xml/LibXML*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/util/*.cpp 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/util/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/validator/*.cpp 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/validator/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/validator/constraints/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/validator/constraints/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/units/*.cpp 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/units/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/math/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/math/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/annotation/*.cpp 
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/annotation/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/sbml/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/sbml/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/sbml/layout/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/sbml/layout/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/common/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/common/*.c
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/compress/CompressCommon.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/compress/InputDecompressor.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src/compress/OutputCompressor.cpp
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
   FILE( GLOB LIBXML_LIB_FILES ${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/win32/libxml2/lib/*.dll DESTINATION bin/../ )
   INSTALL(FILES ${LIBXML_LIB_FILES} DESTINATION bin/../)
   INSTALL(TARGETS sbml DESTINATION bin/../)
ELSE ( WIN32 )
   INSTALL(TARGETS sbml LIBRARY DESTINATION bin/../)
ENDIF( WIN32 )

