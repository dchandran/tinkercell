SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

IF (WIN32)

	SET(BUILD_OS WIN32)
	
	IF (MINGW)
		SET (LIBXML2_LIBRARIES ${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/win32/libxml2/lib/libxml2.dll.a)
	ELSE(MINGW)
		SET (LIBXML2_LIBRARIES ${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/win32/libxml2/lib/libxml2.lib)
	ENDIF(MINGW)
	SET (LIBXML2_INCLUDE_DIR ${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/win32/libxml2/include)

ELSE(WIN32)	
	
	IF (UNIX)
		IF (APPLE)
			SET(BUILD_OS MAC)
		ELSEIF(APPLE)
			SET(BUILD_OS LINUX)
		ENDIF(APPLE)
	ENDIF(UNIX)
	
	FIND_PACKAGE(LibXml2)	

ENDIF(WIN32)

INCLUDE_DIRECTORIES( 
	BEFORE
	${LIBXML2_INCLUDE_DIR}
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml/sbml
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi
	${TINKERCELL_SOURCE_DIR}/ExternalCode/raptor2/src
	${TINKERCELL_SOURCE_DIR}/ExternalCode/raptor2/librdfa
	${TINKERCELL_SOURCE_DIR}/ExternalCode/raptor2/util
)

ADD_DEFINITIONS(-DWITH_CSPMETHOD)

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

FILE( GLOB copasi_SRC_FILES
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/compareExpressions/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/CopasiDataModel/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/crosssection/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/elementaryFluxModes/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/function/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/layout/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/libs/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/lyap/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/MIRIAM/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/model/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/moieties/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/odepack++/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/optimization/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/parameterFitting/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/plot/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/randomGenerator/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/report/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/sbml/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/sbmlunit/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/scan/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/sensitivities/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/steadystate/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/trajectory/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/tss/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/tssanalysis/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/utilities/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/xml/*.cpp
)

ADD_LIBRARY( copasi
  SHARED
  ${copasi_SRC_FILES}
)

TARGET_LINK_LIBRARIES( copasi
  copasi
  sbml
  raptor
  ${LIBXML2_LIBRARIES}
)

IF ( WIN32 )
   INSTALL(TARGETS copasi DESTINATION bin/../)
ELSE ( WIN32 )
   INSTALL(TARGETS copasi LIBRARY DESTINATION bin/../)
ENDIF( WIN32 )

