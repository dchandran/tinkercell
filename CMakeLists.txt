PROJECT( TINKERCELL )

#--------------------------------------------------------------------------
#  CMake not supporting Win32 MinGW?
#--------------------------------------------------------------------------

IF( WIN32 AND MINGW)
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)
  SET(CMAKE_RC_COMPILE_OBJECT
    "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
ENDIF( WIN32 AND MINGW)

#--------------------------------------------------------------------------
#  Build type
#--------------------------------------------------------------------------
OPTION( USE_DEBUG_MODE "Release vs Debug mode" OFF)
IF (USE_DEBUG_MODE)
	SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type")
ELSE(USE_DEBUG_MODE)
	SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type")
ENDIF(USE_DEBUG_MODE)

#--------------------------------------------------------------------------
#  Special compile flags for Qt
#--------------------------------------------------------------------------

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_DEBUG -DQT_XML_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_THREAD_SUPPORT")

#--------------------------------------------------------------------------
#  Special link flags for MS Windows
#--------------------------------------------------------------------------
IF ( WIN32 )
   SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-auto-import")
   OPTION( USE_STDCALL "Use this to enable stdcall instead of cdecl calling convention. Use this if you want to use a library in MS applications. This might cause issues in some libraries, such as embedded python" OFF)
   IF (USE_STDCALL)
		SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--enable-auto-import,--add-stdcall-alias,--out-implib")
	ELSE(USE_STDCALL)
		SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--enable-auto-import")
	ENDIF(USE_STDCALL)
ENDIF ( WIN32 )

#--------------------------------------------------------------------------
# TinkerCell version obtained from Subversion revision number
#--------------------------------------------------------------------------

SET( TINKERCELL_MAJOR_VERSION "1" )

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
SET( USER_HOME_DIR $ENV{HOME} )

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
  SET (CMAKE_OSX_ARCHITECTURES ppc;i386) 
ENDIF( APPLE )

#---------------------------------------------------------------------------
# Define EXECUTABLE_OUTPUT_PATH and LIBRARY_OUTPUT_PATH
#---------------------------------------------------------------------------
SET(EXECUTABLE_OUTPUT_PATH
 ${TINKERCELL_BINARY_DIR}/bin
 CACHE PATH "Single output directory for building all executables."
)
IF (WIN32)
   SET(TINKERCELL_CPP_PLUGINS_FOLDER 
         plugins/windows 
         CACHE PATH "Single output directory for building all cpp libraries." FORCE)
   SET(TINKERCELL_C_PLUGINS_FOLDER 
         plugins/c/windows 
         CACHE PATH "Single output directory for building all c libraries." FORCE)
ELSE(WIN32)
   IF (APPLE)
        SET(TINKERCELL_CPP_PLUGINS_FOLDER 
                   plugins/mac
                    CACHE PATH "Single output directory for building all cpp libraries." FORCE)
        SET(TINKERCELL_C_PLUGINS_FOLDER 
                    plugins/c/mac
                    CACHE PATH "Single output directory for building all c libraries." FORCE)
   ELSE(APPLE)
        OPTION(HAVE_64_BIT "64 bit OS" OFF)
        IF( HAVE_64_BIT )
                MESSAGE(STATUS "64-bit OS")
                SET(TINKERCELL_CPP_PLUGINS_FOLDER 
                             plugins/ubuntu64
                              CACHE STRING "Single output directory for building all cpp libraries." FORCE)
                SET(TINKERCELL_C_PLUGINS_FOLDER
                             plugins/c/ubuntu64
                              CACHE STRING "Single output directory for building all c libraries." FORCE)
       ELSE( HAVE_64_BIT )
                MESSAGE(STATUS "32-bit OS")
                SET(TINKERCELL_CPP_PLUGINS_FOLDER
                             plugins/ubuntu32
                              CACHE STRING "Single output directory for building all cpp libraries." FORCE)
                SET(TINKERCELL_C_PLUGINS_FOLDER
                             plugins/c/ubuntu32
                              CACHE STRING "Single output directory for building all c libraries." FORCE)
       ENDIF( HAVE_64_BIT )
   ENDIF(APPLE)
ENDIF(WIN32)

SET(CPP_LIBRARY_OUTPUT_PATH ${TINKERCELL_BINARY_DIR}/bin/${TINKERCELL_CPP_PLUGINS_FOLDER}
         CACHE PATH "Single output directory for building all cpp libraries." FORCE)
SET(C_LIBRARY_OUTPUT_PATH ${TINKERCELL_BINARY_DIR}/bin/${TINKERCELL_C_PLUGINS_FOLDER}
         CACHE PATH "Single output directory for building all c libraries." FORCE)

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
IF( QT4_FOUND AND QT_VERSION_MINOR GREATER 5)
  
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
    ${TINKERCELL_SOURCE_DIR}/Core/plots
    ${TINKERCELL_SOURCE_DIR}/Core/fileIO
    ${TINKERCELL_SOURCE_DIR}/Core/plugins
    ${TINKERCELL_SOURCE_DIR}/Core/interpreters
    ${TINKERCELL_SOURCE_DIR}/Core/coding
 )

IF (UNIX AND NOT APPLE)
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lrt")
	SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -lrt")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lrt")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
ENDIF(UNIX AND NOT APPLE)

SET(TINKERCELL_BINARY_BIN_DIR ${TINKERCELL_BINARY_DIR}/bin)

#----------------------------------------------------------------------------
#   TinkerCellCore and graphics files
#----------------------------------------------------------------------------

MESSAGE( STATUS "======  configuring external code  ====== ")
ADD_SUBDIRECTORY( ExternalCode )  #MuParser, QWT, cvODE, letc.
MESSAGE( STATUS "==== done configuring external code =====\n")

OPTION( PYTHON_ENABLE "Embed Python in TinkerCell" ON ) #integrate python or not?
OPTION( OCTAVE_ENABLE "Embed Octave in TinkerCell" ON ) #octave?

ADD_SUBDIRECTORY( Core )   #C++ Core + API
ADD_SUBDIRECTORY( API )   #C API + SWIG file
ADD_SUBDIRECTORY( icons )   #icons
ADD_SUBDIRECTORY( Graphics )   #graphics files

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

#----------------------------------------------------------------------------
# Copyright and other files
#----------------------------------------------------------------------------

FILE(GLOB TINKERCELL_SUPPORTING_FILES 
         ${TINKERCELL_SOURCE_DIR}/ABOUT.TXT
         ${TINKERCELL_SOURCE_DIR}/COPYRIGHT.TXT
         ${TINKERCELL_SOURCE_DIR}/UPDATES.TXT)
INSTALL(FILES ${TINKERCELL_SUPPORTING_FILES} DESTINATION bin/../)  

#----------------------------------------------------------------------------
#   TinkerCell main executable
#----------------------------------------------------------------------------

ADD_SUBDIRECTORY( Main )   #main executable
ADD_SUBDIRECTORY( NodeGraphicsItem )   #program for drawing new graphics items

#----------------------------------------------------------------------------
#   Embedding other languages
#----------------------------------------------------------------------------

IF (PYTHON_ENABLE)
	ADD_SUBDIRECTORY( python )
ENDIF (PYTHON_ENABLE)

IF (OCTAVE_ENABLE)
	ADD_SUBDIRECTORY( octave )
ENDIF (OCTAVE_ENABLE)

#the following languages are not supported yet
#ADD_SUBDIRECTORY( perl )
#ADD_SUBDIRECTORY( ruby )
#ADD_SUBDIRECTORY( R )

#----------------------------------------------------------------------------
#   Required
#----------------------------------------------------------------------------
ADD_SUBDIRECTORY( DB )
ADD_SUBDIRECTORY( c )
  
#----------------------------------------------------------------------------
# CPack
#----------------------------------------------------------------------------

INCLUDE( CMake/TinkercellCPack.cmake )

IF (NOT DEFINED TINKERCELL_PACKAGE_FOLDER)
	IF( WIN32 )
		SET(TINKERCELL_PACKAGE_FOLDER _CPack_Packages/win32/ZIP/TinkerCell CACHE STRING "Change this if your CPack package folder is different (depending on CMake version)")
	ENDIF(WIN32)

	IF (APPLE)
	   SET(TINKERCELL_PACKAGE_FOLDER bin CACHE STRING "Change this if your CPack package folder is different (depending on CMake version)")
	ENDIF(APPLE)

	IF (UNIX AND NOT APPLE)
	   SET(TINKERCELL_PACKAGE_FOLDER _CPack_Packages/Linux/TGZ/TinkerCell CACHE STRING "Change this if your CPack package folder is different (depending on CMake version)")
	ENDIF(UNIX AND NOT APPLE)
ENDIF (NOT DEFINED TINKERCELL_PACKAGE_FOLDER)
MESSAGE(STATUS "Package will be placed in ${TINKERCELL_BINARY_DIR}/${TINKERCELL_PACKAGE_FOLDER}")

#--------------------------------------------------------------------------------------------
# OS Specific
#--------------------------------------------------------------------------------------------

IF( WIN32 )
    ADD_SUBDIRECTORY( win32 )  #for Windows (with MinGW)
ENDIF(WIN32)

IF (APPLE)
   ADD_SUBDIRECTORY( mac )   #for Mac
ENDIF(APPLE)

IF (UNIX AND NOT APPLE)
   ADD_SUBDIRECTORY( linux )   #for Linux (tested on Ubuntu)
ENDIF(UNIX AND NOT APPLE)

# ---------------------
# CTEST 
# INCLUDE( CTest )
# --------------------
ELSEIF(  QT4_FOUND AND QT_VERSION_MINOR GREATER 5  )
    MESSAGE(FATAL_ERROR "Could not find Qt 4.5 or greater")
ENDIF(  QT4_FOUND AND QT_VERSION_MINOR GREATER 5  )

