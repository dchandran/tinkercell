#------------------------------------------------------------------------------
#    INCLUDE DIRECTORIES (in addition to those listed in ../CMakeLists.txt)
#------------------------------------------------------------------------------

INCLUDE_DIRECTORIES( 
	BEFORE
	${CMAKE_CURRENT_SOURCE_DIR}/../expat/
	${CMAKE_CURRENT_SOURCE_DIR}/../expat/lib
	${CMAKE_CURRENT_SOURCE_DIR}/../sbml/src
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../raptor/src
	${CMAKE_CURRENT_SOURCE_DIR}/../raptor/librdfa
	${CMAKE_CURRENT_SOURCE_DIR}/../raptor/util
	${CMAKE_CURRENT_SOURCE_DIR}/../antimony
	${CMAKE_CURRENT_SOURCE_DIR}/../galib/ga
	${CMAKE_CURRENT_SOURCE_DIR}/../galib
	${CMAKE_CURRENT_SOURCE_DIR}/../libstructural/include/
)

#------------------------------------------------------------------------------
#    DEFINITIONS FOR COPASI 
#------------------------------------------------------------------------------

#clapack seems to be work better for cross-platform compilation
#ADD_DEFINITIONS(-DUSE_LAPACK)  
ADD_DEFINITIONS(-DUSE_CLAPACK)

#Use the following two definitions to debug copasi
#ADD_DEFINITIONS(-DCOPASI_DEBUG)
#ADD_DEFINITIONS(-DDEBUG_MATRIX)

ADD_DEFINITIONS(-DWITH_CSPMETHOD)
ADD_DEFINITIONS(-DTC_EXPORTS) #export tc_matrix and related tc_* functions
ADD_DEFINITIONS(-DSBW_EXPORTS)
ADD_DEFINITIONS(-DLIBSBML_EXPORTS)
ADD_DEFINITIONS(-DENABLE_LAYOUT)

#------------------------------------------------------------------------------
#  DEFINITIONS FOR WINDOWS
#------------------------------------------------------------------------------

IF (WIN32)
	ADD_DEFINITIONS(-DWIN32)
	ADD_DEFINITIONS(-D_WIN32)
	ADD_DEFINITIONS(-D_WINDOWS)
	ADD_DEFINITIONS(-D_USRDLL)
	ADD_DEFINITIONS(-D_EXPORTS)
	IF (MINGW)       #these definitions appear to be missing in the new MinGW
		ADD_DEFINITIONS(-DCYGWIN)
		ADD_DEFINITIONS(-D__CYGWIN__)
		ADD_DEFINITIONS(-DDBL_EPSILON=2.22045e-16)
		ADD_DEFINITIONS(-DDBL_MIN=1e-999)
		ADD_DEFINITIONS(-DDBL_MIN_EXP=-999)
		ADD_DEFINITIONS(-DDBL_MAX=1.79769e+308)
		IF (USE_STDCALL)
			ADD_DEFINITIONS(-DUSE_STDCALL)      #std instead of cdecl
		 ENDIF(USE_STDCALL)
	ELSE(MINGW)
		ADD_DEFINITIONS(-Dstrcasecmp=_stricmp)
		ADD_DEFINITIONS(-Dstrncasecmp=_strnicmp)
	ENDIF(MINGW)
ENDIF (WIN32)
 IF (APPLE)
  	 ADD_DEFINITIONS(-D_APPLE)
ENDIF(APPLE)

#------------------------------------------------------------------------------
#  COPASI SOURCE FILES
#------------------------------------------------------------------------------

FILE( GLOB copasi_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/model/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/compareExpressions/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CopasiDataModel/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/crosssection/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/elementaryFluxModes/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/function/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/layout/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/libs/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/lyap/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/moieties/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/odepack++/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/optimization/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/parameterFitting/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/plot/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/randomGenerator/*.cpp	
	${CMAKE_CURRENT_SOURCE_DIR}/sbml/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sbmlunit/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/scan/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sensitivities/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/steadystate/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/report/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/trajectory/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tss/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tssanalysis/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/utilities/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/xml/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/commandline/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MIRIAM/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MIRIAM/WebServicesIssues/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/copasi_api.cpp
	#${CMAKE_CURRENT_SOURCE_DIR}/../TC_structs.c
	${TINKERCELL_SOURCE_DIR}/API/TC_structs.c
	${CMAKE_CURRENT_SOURCE_DIR}/mtrand.c
)

ADD_DEFINITIONS(-DHAVE_EXPAT_CONFIG_H)
FILE( GLOB  EXPAT_SRC_FILES   ${CMAKE_CURRENT_SOURCE_DIR}/../expat/lib/*.c)

#--------------------------------
#  MAKE LIBRARY libcopasi
#-------------------------------

ADD_LIBRARY( copasi
  SHARED
  ${copasi_SRC_FILES}
  ${EXPAT_SRC_FILES}
)

#---------------------------------------------------------------------
# This section is for resolving complications in Viisual Studio
#  Visual Studio automatically renames the boost library 
#  when linking
#---------------------------------------------------------------------
IF (WIN32 AND NOT MINGW)
	SET( boost_library_name libboost_regex-vc100-mt-1_48)
	IF(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		SET( boost_library_name libboost_regex-vc100-mt-gd-1_48)
	ENDIF()
ELSE (WIN32 AND NOT MINGW)
	SET( boost_library_name boost_regex)
ENDIF (WIN32 AND NOT MINGW)
#----------------------------------------------------------------------

TARGET_LINK_LIBRARIES( copasi
  copasi
  sbml
  raptor
  lapack
  blas
  f2c
  antimony
  galib
  structural
  muparser
  ${boost_library_name}
)

#----------------------------------
#  INSTALL libcopasi in . (CPack)
#----------------------------------

INSTALL(TARGETS copasi
	RUNTIME DESTINATION bin/../
	LIBRARY DESTINATION bin/../
	ARCHIVE DESTINATION bin/../
)

