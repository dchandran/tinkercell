PROJECT( TINKERCELL )

SET(CMAKE_BUILD_TYPE "Release")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_DEBUG -DQT_XML_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_THREAD_SUPPORT")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --export-all-symbols")

SET( TINKERCELL_MAJOR_VERSION "1" )
SET( TINKERCELL_MINOR_VERSION "0" )

# just in case someone wants
# 1- to mirror using something different than svn
# 2- to build it from tarball
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

SET( TINKERCELL_VERSION
"${TINKERCELL_MAJOR_VERSION}.${TINKERCELL_MINOR_VERSION}.${TINKERCELL_WC_REVISION}" )

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
# BUNDLE Application for Mac
#---------------------------------------------------------------------------
IF( APPLE )
  OPTION( BUILD_BUNDLE "Make A Bundle Application" ON )
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
# Build only if the version of qt is newer
# than Qt4.4. Note Qt starts being LGPL with
# Qt4.5
# IF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )
IF( QT4_FOUND AND OPENGL_FOUND )
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

  LINK_DIRECTORIES(
    ${CPP_LIBRARY_OUTPUT_PATH}
    ${C_LIBRARY_OUTPUT_PATH}
    ${TINKERCELL_BINARY_DIR}
    ${TINKERCELL_BINARY_DIR}/bin
	${TINKERCELL_BINARY_DIR}/ExternalCode/
	${TINKERCELL_BINARY_DIR}/ExternalCode/lapack
    ${TINKERCELL_BINARY_DIR}/ExternalCode/muparser/
    ${TINKERCELL_BINARY_DIR}/ExternalCode/qwt/
    ${TINKERCELL_BINARY_DIR}/ExternalCode/qwtplot3d/include/
    ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode260/
    ${TINKERCELL_BINARY_DIR}/ExternalCode/optim/
    ${TINKERCELL_BINARY_DIR}/ExternalCode/sbml
    ${TINKERCELL_BINARY_DIR}/ExternalCode/Antimony
    ${TINKERCELL_BINARY_DIR}/c/
    ${TINKERCELL_BINARY_DIR}/c/lpsolve/
    ${TINKERCELL_BINARY_DIR}/c/sundials/
    ${TINKERCELL_BINARY_DIR}/c/nvector/
    ${TINKERCELL_BINARY_DIR}/c/cvode/
    ${TINKERCELL_BINARY_DIR}/Core/
    ${TINKERCELL_BINARY_DIR}/BasicTools/
    ${TINKERCELL_BINARY_DIR}/ModuleTools/
    ${TINKERCELL_BINARY_DIR}/ModelingTools/
    ${TINKERCELL_BINARY_DIR}/OtherTools/
    ${TINKERCELL_BINARY_DIR}/AntimonyEditor/
    ${TINKERCELL_BINARY_DIR}/NodeGraphicsItem/
    ${TINKERCELL_BINARY_DIR}/NodesTree/
    ${TINKERCELL_BINARY_DIR}/PlotTools/
    ${TINKERCELL_BINARY_DIR}/SegmentationAnimation/
    ${TINKERCELL_BINARY_DIR}/DynamicCodeTools/
    ${TINKERCELL_BINARY_DIR}/GeneticNetworks/
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
		${TINKERCELL_BINARY_DIR}/ExternalCode/Release
		${TINKERCELL_BINARY_DIR}/ExternalCode/Debug
		${TINKERCELL_BINARY_DIR}/ExternalCode/lapack/Release
		${TINKERCELL_BINARY_DIR}/ExternalCode/lapack/Debug
		${TINKERCELL_BINARY_DIR}/ExternalCode/muparser/Release
		${TINKERCELL_BINARY_DIR}/ExternalCode/muparser/Debug
		${TINKERCELL_BINARY_DIR}/ExternalCode/qwt/Release
		${TINKERCELL_BINARY_DIR}/ExternalCode/qwt/Debug
		${TINKERCELL_BINARY_DIR}/ExternalCode/qwtplot3d/include/Release
		${TINKERCELL_BINARY_DIR}/ExternalCode/qwtplot3d/include/Debug
		${TINKERCELL_BINARY_DIR}/ExternalCode/cvode_src/cvode/Release
		${TINKERCELL_BINARY_DIR}/ExternalCode/cvode_src/cvode/Debug
		${TINKERCELL_BINARY_DIR}/ExternalCode/cvode_src/sundials/Release
		${TINKERCELL_BINARY_DIR}/ExternalCode/cvode_src/sundials/Debug
		${TINKERCELL_BINARY_DIR}/ExternalCode/nvec_par/Release
		${TINKERCELL_BINARY_DIR}/ExternalCode/nvec_par/Debug
		${TINKERCELL_BINARY_DIR}/ExternalCode/optim/Release
		${TINKERCELL_BINARY_DIR}/ExternalCode/optim/Debug
		${TINKERCELL_BINARY_DIR}/ExternalCode/sbml/Release
		${TINKERCELL_BINARY_DIR}/ExternalCode/sbml/Debug
		${TINKERCELL_BINARY_DIR}/ExternalCode/Antimony/Release
		${TINKERCELL_BINARY_DIR}/ExternalCode/Antimony/Debug
		${TINKERCELL_BINARY_DIR}/c/Release
		${TINKERCELL_BINARY_DIR}/c/Debug
		${TINKERCELL_BINARY_DIR}/c/lpsolve/Release
		${TINKERCELL_BINARY_DIR}/c/lpsolve/Debug
		${TINKERCELL_BINARY_DIR}/c/sundials/Release
		${TINKERCELL_BINARY_DIR}/c/sundials/Debug
		${TINKERCELL_BINARY_DIR}/c/nvector/Release
		${TINKERCELL_BINARY_DIR}/c/nvector/Debug
		${TINKERCELL_BINARY_DIR}/c/cvode/Release
		${TINKERCELL_BINARY_DIR}/c/cvode/Debug
		${TINKERCELL_BINARY_DIR}/Core/Release
		${TINKERCELL_BINARY_DIR}/Core/Debug
		${TINKERCELL_BINARY_DIR}/BasicTools/Release
		${TINKERCELL_BINARY_DIR}/BasicTools/Debug
		${TINKERCELL_BINARY_DIR}/ModuleTools/Release
		${TINKERCELL_BINARY_DIR}/ModuleTools/Debug
		${TINKERCELL_BINARY_DIR}/ModelingTools/Release
		${TINKERCELL_BINARY_DIR}/ModelingTools/Debug
		${TINKERCELL_BINARY_DIR}/OtherTools/Release
		${TINKERCELL_BINARY_DIR}/OtherTools/Debug
		${TINKERCELL_BINARY_DIR}/AntimonyEditor/Release
		${TINKERCELL_BINARY_DIR}/AntimonyEditor/Debug
		${TINKERCELL_BINARY_DIR}/NodeGraphicsItem/Release
		${TINKERCELL_BINARY_DIR}/NodeGraphicsItem/Debug
		${TINKERCELL_BINARY_DIR}/NodesTree/Release
		${TINKERCELL_BINARY_DIR}/NodesTree/Debug
		${TINKERCELL_BINARY_DIR}/PlotTools/Release
		${TINKERCELL_BINARY_DIR}/PlotTools/Debug
		${TINKERCELL_BINARY_DIR}/SegmentationAnimation/Release
		${TINKERCELL_BINARY_DIR}/SegmentationAnimation/Debug
		${TINKERCELL_BINARY_DIR}/DynamicCodeTools/Release
		${TINKERCELL_BINARY_DIR}/DynamicCodeTools/Debug
		${TINKERCELL_BINARY_DIR}/GeneticNetworks/Release
		${TINKERCELL_BINARY_DIR}/GeneticNetworks/Debug
	   )
  ENDIF ( WIN32 )

  INCLUDE_DIRECTORIES( BEFORE
    ${QT_INCLUDES}
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/muparser/
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/qwt/
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/optim/
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/lapack/INCLUDE
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode260/include
    ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode260/include
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/Antimony
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml_odeSolver/src
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbw/include
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbw/include/SBW
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbw/include/libxml
    ${TINKERCELL_SOURCE_DIR}/c/
    ${TINKERCELL_SOURCE_DIR}/c/lpsolve/
    ${TINKERCELL_SOURCE_DIR}/Core/
    ${TINKERCELL_SOURCE_DIR}/BasicTools/
    ${TINKERCELL_SOURCE_DIR}/ModuleTools/
    ${TINKERCELL_SOURCE_DIR}/ModelingTools/
    ${TINKERCELL_SOURCE_DIR}/OtherTools/
    ${TINKERCELL_SOURCE_DIR}/AntimonyEditor/
    ${TINKERCELL_SOURCE_DIR}/NodeGraphicsItem/
    ${TINKERCELL_SOURCE_DIR}/NodesTree/
    ${TINKERCELL_SOURCE_DIR}/PlotTools/
    ${TINKERCELL_SOURCE_DIR}/SegmentationAnimation/
    ${TINKERCELL_SOURCE_DIR}/DynamicCodeTools/
    ${TINKERCELL_SOURCE_DIR}/GeneticNetworks/
 )

IF (UNIX AND NOT APPLE)
	SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lrt")
	SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -lrt")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lrt")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
ENDIF(UNIX AND NOT APPLE)

#----------------------------------------------------------------------------
#   Tinkercell plugins and other components
#----------------------------------------------------------------------------

  SET(TINKERCELL_BINARY_BIN_DIR TINKERCELL_BINARY_DIR/bin)
  
  ADD_SUBDIRECTORY( Core )
  ADD_SUBDIRECTORY( Main )
  ADD_SUBDIRECTORY( ExternalCode )  
  ADD_SUBDIRECTORY( NodeGraphicsItem )
  ADD_SUBDIRECTORY( NodesTree )
  ADD_SUBDIRECTORY( BasicTools )
  ADD_SUBDIRECTORY( PlotTools )
  ADD_SUBDIRECTORY( DynamicCodeTools )
  ADD_SUBDIRECTORY( ModuleTools )
  ADD_SUBDIRECTORY( ModelingTools )
  ADD_SUBDIRECTORY( OtherTools )
  ADD_SUBDIRECTORY( ArrowItems )
  ADD_SUBDIRECTORY( NodeItems )
  ADD_SUBDIRECTORY( OtherItems )
  ADD_SUBDIRECTORY( Documentation )
  ADD_SUBDIRECTORY( AntimonyEditor )
  ADD_SUBDIRECTORY( GeneticNetworks )
  ADD_SUBDIRECTORY( DB )
  ADD_SUBDIRECTORY( c )
  
#---------------------------------------------------------------------------
# Doxygen documentation
#---------------------------------------------------------------------------
OPTION( DOXYGENDOC "Generate Doxygen Documentation" OFF )

#---------------------------------------------------------------------------
# SWIG
#---------------------------------------------------------------------------

SET_PROPERTY(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS)
ADD_SUBDIRECTORY( python )
ADD_SUBDIRECTORY( ruby )
ADD_SUBDIRECTORY( perl )
ADD_SUBDIRECTORY( octave )
ADD_SUBDIRECTORY( R )


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
# Done with Tinkercell plugins and other components
#----------------------------------------------------------------------------

INCLUDE( CMake/TinkercellCPack.cmake )

# ---------------------
# CTEST 
# INCLUDE( CTest )
# --------------------


ENDIF( QT4_FOUND AND OPENGL_FOUND  )
