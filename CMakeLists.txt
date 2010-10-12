PROJECT( TINKERCELL )

SET(CMAKE_BUILD_TYPE "Release")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_DEBUG -DQT_XML_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_THREAD_SUPPORT")

IF ( WIN32 )
   SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--enable-auto-import")
   SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-auto-import")
ENDIF ( WIN32 )

#--------------------------------------------------------------------------
# TinkerCell version obtained from Subversion revision number
#--------------------------------------------------------------------------

#SET( TINKERCELL_MAJOR_VERSION "1" )
#SET( TINKERCELL_MINOR_VERSION "0" )

IF( EXISTS ${CMAKE_SOURCE_DIR}/.svn )
  FIND_PACKAGE( Subversion )

  IF( Subversion_FOUND )
    Subversion_WC_INFO( ${TINKERCELL_SOURCE_DIR} TINKERCELL )
  ELSE( Subversion_FOUND )
    SET( TINKERCELL_WC_REVISION "0" )
  ENDIF( Subversion_FOUND )
ELSE( EXISTS ${CMAKE_SOURCE_DIR}/.svn )
  SET( TINKERCELL_WC_REVISION "0" )
ENDIF( EXISTS ${CMAKE_SOURCE_DIR}/.svn )

SET( TINKERCELL_VERSION ${TINKERCELL_WC_REVISION} )

#--------------------------------------------------------------------------
# cmake policies
#--------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

IF( COMMAND CMAKE_POLICY )
  CMAKE_POLICY( VERSION 2.4 )
  CMAKE_POLICY( SET CMP0005 NEW )
  CMAKE_POLICY( SET CMP0003 NEW )
# CMAKE_POLICY( SET CMP0002 OLD )
   IF (POLICY CMP0015)
      CMAKE_POLICY( SET CMP0015 OLD )
   ENDIF(POLICY CMP0015)
ENDIF(COMMAND CMAKE_POLICY)

#---------------------------------------------------------------------------
# Mac
#---------------------------------------------------------------------------
IF( APPLE )
  OPTION( BUILD_BUNDLE "Make A Bundle Application" ON )
  SET (CMAKE_OSX_ARCHITECTURES "ppc;i386;x86_64;ppc64;" CACHE STRING "Rogue") 
ENDIF( APPLE )

#---------------------------------------------------------------------------
# Define EXECUTABLE_OUTPUT_PATH and LIBRARY_OUTPUT_PATH
#---------------------------------------------------------------------------
SET(EXECUTABLE_OUTPUT_PATH
 ${TINKERCELL_BINARY_DIR}/bin
 CACHE PATH "Single output directory for building all executables."
)
SET(CPP_LIBRARY_OUTPUT_PATH
 ${TINKERCELL_BINARY_DIR}/bin/plugins/
 CACHE PATH "Single output directory for building all cpp libraries."
)
SET(C_LIBRARY_OUTPUT_PATH
 ${TINKERCELL_BINARY_DIR}/bin/plugins/c/
 CACHE PATH "Single output directory for building all c libraries."
)

#----------------
# BOOST library
#----------------

#find_package( Boost 1.41 COMPONENTS threads regex)
#set(Boost_USE_STATIC_LIBS   ON)
#set(Boost_USE_MULTITHREADED ON)
#INCLUDE_DIRECTORIES ( ${Boost_INCLUDE_DIRS} )

#---------------------------------------------------------------------------
# Qt4
#---------------------------------------------------------------------------
FIND_PACKAGE( Qt4 )
FIND_PACKAGE ( OpenGL )
#--------------------------------------------
# Build only if the version of qt is newer than Qt4.5.
IF( QT4_FOUND AND QT_VERSION_MINOR GREATER 5 )
  IF( QT_USE_FILE )
	  SET( QT_USE_QTCORE TRUE )
	  SET( QT_USE_QTGUI TRUE )
	  SET( QT_USE_QTXML TRUE )
	  SET( QT_USE_QTOPENGL TRUE )
      SET( QT_USE_QT3SUPPORT FALSE )
     #SET( QT_USE_QTSVG TRUE )
     INCLUDE( ${QT_USE_FILE} )
  ELSE( QT_USE_FILE )
     SET( QT_LIBRARIES ${QT_QT_LIBRARY} )
  ENDIF( QT_USE_FILE )

  LINK_DIRECTORIES( BEFORE
    ${CPP_LIBRARY_OUTPUT_PATH}
    ${C_LIBRARY_OUTPUT_PATH}
    ${TINKERCELL_BINARY_DIR}
    ${TINKERCELL_BINARY_DIR}/bin
   )
  IF ( WIN32 )
	  LINK_DIRECTORIES( 
	    BEFORE
		${CPP_LIBRARY_OUTPUT_PATH}/Release
		${CPP_LIBRARY_OUTPUT_PATH}/Debug
		${C_LIBRARY_OUTPUT_PATH}/Release
		${C_LIBRARY_OUTPUT_PATH}/Debug
		${TINKERCELL_BINARY_DIR}/Release
		${TINKERCELL_BINARY_DIR}/Debug
		${TINKERCELL_BINARY_DIR}/bin/Release
		${TINKERCELL_BINARY_DIR}/bin/Debug	
	   )
  ENDIF ( WIN32 )

  INCLUDE_DIRECTORIES( BEFORE
    ${QT_INCLUDES}
    ${TINKERCELL_SOURCE_DIR}
    ${TINKERCELL_SOURCE_DIR}/ExternalCode
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/muparser
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/qwt
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/optim
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/lapack/INCLUDE
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode260/include
    ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode260/include
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode260/include/cvode
    ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode260/include/cvode
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode260/include/sundials
    ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode260/include/sundials
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode260/include/nvector
    ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode260/include/nvector
    ${TINKERCELL_SOURCE_DIR}/c
    ${TINKERCELL_SOURCE_DIR}/API
    ${TINKERCELL_SOURCE_DIR}/Core
 )

IF (UNIX AND NOT APPLE)
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lrt")
	SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -lrt")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lrt")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
ENDIF(UNIX AND NOT APPLE)

SET(TINKERCELL_BINARY_BIN_DIR ${TINKERCELL_BINARY_DIR}$/bin)

#----------------------------------------------------------------------------
#   TinkerCellCore and graphics files
#----------------------------------------------------------------------------

ADD_SUBDIRECTORY( Core )   #C++ Core + API
ADD_SUBDIRECTORY( API )   #C API + SWIG file
ADD_SUBDIRECTORY( Main )   #main executable
ADD_SUBDIRECTORY( ExternalCode )  #MuParser, QWT, cvODE, letc.
ADD_SUBDIRECTORY( icons )   #icons
ADD_SUBDIRECTORY( Graphics )   #graphics files
ADD_SUBDIRECTORY( NodeGraphicsItem )   #program for drawing new graphics items
IF (APPLE)
   ADD_SUBDIRECTORY( mac )   #Only for Mac
ENDIF(APPLE)
#---------------------------------------------------------------------------
# documentation
#---------------------------------------------------------------------------

ADD_SUBDIRECTORY( Documentation )

#----------------------------------------------------------------------------
# Other programs (not really part of Tinkercell)
#----------------------------------------------------------------------------

OPTION( BUILD_EXAMPLE_PROGRAMS "SimpleDesigner, Multicell, SegmentationAnimation" OFF )

IF( BUILD_EXAMPLE_PROGRAMS )
     ADD_SUBDIRECTORY( Examples )
ENDIF( BUILD_EXAMPLE_PROGRAMS )

#--------------------------------------------------------------------------------------------
# Windows only: include specific binaries that are usually already available on Unix systems
#--------------------------------------------------------------------------------------------

IF( WIN32 )
    ADD_SUBDIRECTORY( win32 )
ENDIF(WIN32)

#----------------------------------------------------------------------------
# Copyright and other files
#----------------------------------------------------------------------------

FILE(GLOB TINKERCELL_SUPPORTING_FILES about.txt COPYRIGHT.TXT *.py)
INSTALL(FILES ${TINKERCELL_SUPPORTING_FILES} DESTINATION bin/../)  

#----------------------------------------------------------------------------
#   TinkerCell plugins
#----------------------------------------------------------------------------

INCLUDE( CMake/TinkercellPlugins.cmake )
  
#----------------------------------------------------------------------------
# CPack
#----------------------------------------------------------------------------

INCLUDE( CMake/TinkercellCPack.cmake )

# ---------------------
# CTEST 
# INCLUDE( CTest )
# --------------------
ELSEIF(  QT4_FOUND AND QT_VERSION_MINOR GREATER 5  )
    MESSAGE(FATAL_ERROR "Could not find Qt 4.5 or greater")
ENDIF(  QT4_FOUND AND QT_VERSION_MINOR GREATER 5  )

