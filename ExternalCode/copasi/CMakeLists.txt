
SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

#---------------------
#    OS STUFF
#---------------------
IF (WIN32)

   SET(BUILD_OS WIN32)

ELSE(WIN32)	
	
	IF (UNIX)
		IF (APPLE)
			SET(BUILD_OS MAC)
		ELSEIF(APPLE)
			SET(BUILD_OS LINUX)
		ENDIF(APPLE)
	ENDIF(UNIX)

ENDIF(WIN32)

#----------------------------
#    INCLUDE DIRECTORIES
#----------------------------

INCLUDE_DIRECTORIES( 
	BEFORE
	${TINKERCELL_SOURCE_DIR}/API
	${TINKERCELL_SOURCE_DIR}/ExternalCode/expat/lib
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
#ADD_DEFINITIONS(-D__cplusplus)
ADD_DEFINITIONS(-DUSE_CLAPACK)
#ADD_DEFINITIONS(-DUSE_LAPACK)
ADD_DEFINITIONS(-DCOPASI_DEBUG)
ADD_DEFINITIONS(-DDEBUG_MATRIX)
ADD_DEFINITIONS(-DWITH_CSPMETHOD)

#------------------------------------------
#  DEFINITIONS FOR WINDOWS
#------------------------------------------

IF (WIN32)
	ADD_DEFINITIONS(-DWIN32)
	ADD_DEFINITIONS(-D_WIN32)
	ADD_DEFINITIONS(-D_WINDOWS)
	ADD_DEFINITIONS(-D_USRDLL)
	ADD_DEFINITIONS(-D_EXPORTS)
	#ADD_DEFINITIONS(-DIN_LIBXML)
	IF (MINGW)
		ADD_DEFINITIONS(-DCYGWIN)
		ADD_DEFINITIONS(-D__CYGWIN__)
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
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/moieties/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/odepack++/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/optimization/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/parameterFitting/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/plot/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/randomGenerator/*.cpp	
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/sbml/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/sbmlunit/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/scan/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/sensitivities/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/steadystate/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/report/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/trajectory/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/tss/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/tssanalysis/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/utilities/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/xml/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/commandline/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/MIRIAM/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/MIRIAM/WebServicesIssues/*.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi/copasi_api.cpp
	${TINKERCELL_SOURCE_DIR}/API/TC_structs.c
)

#--------------------------
#  MAKE libcopasi
#--------------------------

ADD_LIBRARY( copasi
#  STATIC
  SHARED
  ${copasi_SRC_FILES}
)

TARGET_LINK_LIBRARIES( copasi
  copasi
  sbml
  raptor
  expat
  lapack
  blas
  f2c
)

#----------------------------------
#  INSTALL libcopasi in . (CPack)
#----------------------------------

IF ( WIN32 )
   INSTALL(TARGETS copasi DESTINATION bin/../)
ELSE ( WIN32 )
   INSTALL(TARGETS copasi LIBRARY DESTINATION bin/../)
ENDIF( WIN32 )

#----------------------------------
#  COPASI api
#----------------------------------

OPTION( TEST_COPASI_API "test copasi api" OFF )

IF (TEST_COPASI_API)
  ADD_EXECUTABLE(copasi_api_test test_copasi.c)

  TARGET_LINK_LIBRARIES( copasi_api_test
    copasi
    sbml
    raptor
    #expat
    #lapack
    #blas
    #f2c
  )

ENDIF (TEST_COPASI_API)

