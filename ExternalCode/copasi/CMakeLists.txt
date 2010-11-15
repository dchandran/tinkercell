SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

#---------------------
#    XML PARSER
#---------------------
IF (WIN32)

   SET(BUILD_OS WIN32)
	
	# libxml2 is included as a dll file in windows, so look in a specific folder:
	IF (MINGW)
		SET (LIBXML2_LIBRARIES ${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/win32/libxml2/lib/libxml2.dll.a)
	ELSE(MINGW)
		SET (LIBXML2_LIBRARIES ${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/win32/libxml2/lib/libxml2.lib)
	ENDIF(MINGW)
	SET (LIBXML2_INCLUDE_DIR ${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/win32/libxml2/include)

ELSE(WIN32)	
	
	IF (UNIX)
		IF (APPLE)
			SET(BUILD_OS MAC)
		ELSEIF(APPLE)
			SET(BUILD_OS LINUX)
		ENDIF(APPLE)
	ENDIF(UNIX)
	
	#ask CMAKE to find libxml2
	FIND_PACKAGE(LibXml2)	

ENDIF(WIN32)

#----------------------------
#    INCLUDE DIRECTORIES
#----------------------------

INCLUDE_DIRECTORIES( 
	BEFORE
	${LIBXML2_INCLUDE_DIR}
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/include
	${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/src
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi
	${TINKERCELL_SOURCE_DIR}/ExternalCode/raptor/src
	${TINKERCELL_SOURCE_DIR}/ExternalCode/raptor/librdfa
	${TINKERCELL_SOURCE_DIR}/ExternalCode/raptor/util
)

#---------------------
#    DEFINITIONS
#---------------------

ADD_DEFINITIONS(-DWITH_CSPMETHOD)
ADD_DEFINITIONS(-DUSE_CLAPACK)
ADD_DEFINITIONS(-DUSE_LAPACK)

#--------------------------
#    DEFINITIONS FOR WINDOWS
#--------------------------

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

#--------------------------
#  COPASI SOURCE FILES
#--------------------------

FILE( GLOB copasi_SRC_FILES
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/model/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/compareExpressions/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/CopasiDataModel/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/crosssection/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/elementaryFluxModes/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/function/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/layout/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/libs/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/lyap/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/MIRIAM/*.cpp
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

#--------------------------
#  MAKE libcopasi
#--------------------------

ADD_LIBRARY( copasi
  SHARED
  ${copasi_SRC_FILES}
)

TARGET_LINK_LIBRARIES( copasi
  copasi
  sbml
  raptor
  lapack
  ${LIBXML2_LIBRARIES}
)

#----------------------------------
#  INSTALL libcopasi in . (CPack)
#----------------------------------

IF ( WIN32 )
   INSTALL(TARGETS copasi DESTINATION bin/../)
ELSE ( WIN32 )
   INSTALL(TARGETS copasi LIBRARY DESTINATION bin/../)
ENDIF( WIN32 )

