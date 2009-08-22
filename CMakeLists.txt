PROJECT( TINKERCELL )

ADD_DEFINITIONS( 
  -DQT_NO_DEBUG -DQT_XML_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_THREAD_SUPPORT
  -DINLINE=static -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine )

SET( TINKERCELL_MAJOR_VERSION "1" )
SET( TINKERCELL_MINOR_VERSION "0" )

FIND_PACKAGE( Subversion )

IF( Subversion_FOUND )
  Subversion_WC_INFO( ${TINKERCELL_SOURCE_DIR} TINKERCELL )
ELSE( Subversion_FOUND )
  SET( TINKERCELL_WC_REVISION "0" )  
ENDIF( Subversion_FOUND )

# arnaudgelas: this MUST be chosen by the developer when running cmake
# and not in this file!
#SET( CMAKE_BUILT_TYPE DEBUG )

SET( TINKERCELL_VERSION
"${TINKERCELL_MAJOR_VERSION}.${TINKERCELL_MINOR_VERSION}.${TINKERCELL_WC_REVISION}" )

#--------------------------------------------------------------------------
# cmake policies
#--------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

IF( COMMAND CMAKE_POLICY )
  CMAKE_POLICY( VERSION 2.4 )
  CMAKE_POLICY( SET CMP0005 OLD )
  CMAKE_POLICY( SET CMP0003 NEW )
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
 ${TINKERCELL_BINARY_DIR}/bin/Plugins/
 CACHE PATH "Single output directory for building all cpp libraries."
)
SET(C_LIBRARY_OUTPUT_PATH
 ${TINKERCELL_BINARY_DIR}/bin/Plugins/c/
 CACHE PATH "Single output directory for building all c libraries."
)

#---------------------------------------------------------------------------
# Doxygen documentation
#---------------------------------------------------------------------------
OPTION( DOXYGENDOC "Generate Doxygen Documentation" OFF )

IF( DOXYGENDOC )

FIND_PACKAGE( Doxygen )

  IF( DOXYGEN_FOUND )
    IF( DOXYGEN_DOT_FOUND )
      ADD_SUBDIRECTORY( Documentation )
    ELSE( DOXYGEN_DOT_FOUND )
      MESSAGE( SEND_ERROR 
        "Dot MUST be installed (visit http://www.graphviz.org/)" )
    ENDIF( DOXYGEN_DOT_FOUND )
  ELSE( DOXYGEN_FOUND )
    MESSAGE( SEND_ERROR
      "Doxygen MUST be installed 
        (visit http://www.stack.nl/~dimitri/doxygen/)" )
  ENDIF( DOXYGEN_FOUND )
ENDIF( DOXYGENDOC )

#---------------------------------------------------------------------------
# Python
#---------------------------------------------------------------------------
FIND_PACKAGE( PythonLibs )

IF( NOT PYTHONLIBS_FOUND )
  MESSAGE( SEND_ERROR "TinkerCell requires python to be installed" )
ENDIF( NOT PYTHONLIBS_FOUND )

#---------------------------------------------------------------------------
# Qt4
#---------------------------------------------------------------------------
FIND_PACKAGE( Qt4 )

#--------------------------------------------
# Build only if the version of qt is newer
# than Qt4.4. Note Qt starts being LGPL with
# Qt4.5
# IF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )
IF( QT4_FOUND )
  IF( QT_USE_FILE )
	SET( QT_USE_QTCORE TRUE )
	SET( QT_USE_QTGUI TRUE )
	SET( QT_USE_QTXML TRUE )
	set( QT_USE_QTOPENGL TRUE )
    #SET( QT_USE_QTSVG TRUE )
    INCLUDE( ${QT_USE_FILE} )
  ELSE( QT_USE_FILE )
    SET( QT_LIBRARIES ${QT_QT_LIBRARY} )
  ENDIF( QT_USE_FILE )

  LINK_DIRECTORIES(
   ${CPP_LIBRARY_OUTPUT_PATH}
   ${TINKERCELL_SOURCE_DIR}/ExternalLib/
   )

  INCLUDE_DIRECTORIES( BEFORE
    ${PYTHON_INCLUDE_PATH}
    ${QT_INCLUDES}
# INCLUDE DIRECTORIES FROM TINKERCELL
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/muparser/
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/qwt/
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode_src/cvode/
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/cvode_src/sundials/
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/nvec_par/
    ${TINKERCELL_SOURCE_DIR}/c/
    ${TINKERCELL_SOURCE_DIR}/c/lpsolve/
    ${TINKERCELL_SOURCE_DIR}/c/lpsolve/shared/
    ${TINKERCELL_SOURCE_DIR}/c/lpsolve/colamd/
    ${TINKERCELL_SOURCE_DIR}/c/lpsolve/bfp/
    ${TINKERCELL_SOURCE_DIR}/c/lpsolve/bfp/bfp_LUSOL
    ${TINKERCELL_SOURCE_DIR}/c/lpsolve/bfp/bfp_LUSOL/LUSOL
    ${TINKERCELL_SOURCE_DIR}/c/sundials/
    ${TINKERCELL_SOURCE_DIR}/c/nvector/
    ${TINKERCELL_SOURCE_DIR}/c/cvode/
    ${TINKERCELL_SOURCE_DIR}/Core/
    ${TINKERCELL_SOURCE_DIR}/BasicTools/
    ${TINKERCELL_SOURCE_DIR}/OtherTools/
    ${TINKERCELL_SOURCE_DIR}/AntimonyEditor/
    ${TINKERCELL_SOURCE_DIR}/AntimonyEditor/include/
    ${TINKERCELL_SOURCE_DIR}/NodeGraphicsItem/
    ${TINKERCELL_SOURCE_DIR}/NodesTree/
    ${TINKERCELL_SOURCE_DIR}/PlotTools/
    ${TINKERCELL_SOURCE_DIR}/ContainerTools/
    ${TINKERCELL_SOURCE_DIR}/SegmentationAnimation/
    ${TINKERCELL_SOURCE_DIR}/CInputWindows/
    ${TINKERCELL_SOURCE_DIR}/DynamicCodeTools/
# includes in BINARY_DIR just in case
    ${TINKERCELL_BINARY_DIR}/ExternalCode/muparser/
    ${TINKERCELL_BINARY_DIR}/ExternalCode/qwt/
    ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode_src/cvode/
    ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode_src/sundials/
    ${TINKERCELL_BINARY_DIR}/ExternalCode/cvode_src/nvec_par/
    ${TINKERCELL_BINARY_DIR}/c/
    ${TINKERCELL_BINARY_DIR}/c/lpsolve/
    ${TINKERCELL_BINARY_DIR}/c/lpsolve/shared/
    ${TINKERCELL_BINARY_DIR}/c/lpsolve/colamd/
    ${TINKERCELL_BINARY_DIR}/c/lpsolve/bfp/
    ${TINKERCELL_BINARY_DIR}/c/lpsolve/bfp/bfp_LUSOL
    ${TINKERCELL_BINARY_DIR}/c/lpsolve/bfp/bfp_LUSOL/LUSOL
    ${TINKERCELL_BINARY_DIR}/Core/
    ${TINKERCELL_BINARY_DIR}/BasicTools/
    ${TINKERCELL_BINARY_DIR}/OtherTools/
    ${TINKERCELL_BINARY_DIR}/NodeGraphicsItem/
    ${TINKERCELL_BINARY_DIR}/NodesTree/
    ${TINKERCELL_BINARY_DIR}/PlotTools/
    ${TINKERCELL_BINARY_DIR}/ContainerTools/
    ${TINKERCELL_BINARY_DIR}/SegmentationAnimation/
    ${TINKERCELL_BINARY_DIR}/CInputWindows/
    ${TINKERCELL_BINARY_DIR}/DynamicCodeTools/
 )

  
  ADD_SUBDIRECTORY( Main )	
  ADD_SUBDIRECTORY( Core )
  ADD_SUBDIRECTORY( c )
  ADD_SUBDIRECTORY( ExternalCode )  
  ADD_SUBDIRECTORY( NodeGraphicsItem )
  ADD_SUBDIRECTORY( NodesTree )
  ADD_SUBDIRECTORY( BasicTools )
  ADD_SUBDIRECTORY( PlotTools )
  ADD_SUBDIRECTORY( DynamicCodeTools )
  ADD_SUBDIRECTORY( ContainerTools )
  ADD_SUBDIRECTORY( OtherTools )
  ADD_SUBDIRECTORY( CInputWindows )
  ADD_SUBDIRECTORY( SegmentationAnimation )
  ADD_SUBDIRECTORY( SimpleDesigner )  
  
#---------------------------------------------------------------------------
# ANTIMONY editor
#---------------------------------------------------------------------------

  OPTION( BUILD_ANTIMONYEDITOR 
    "Build Antimony Editor. Note that it requires to have antimony parser libraries installed." 
    OFF )

  IF( BUILD_ANTIMONYEDITOR )
    ADD_SUBDIRECTORY( AntimonyEditor )
  ENDIF( BUILD_ANTIMONYEDITOR )


#---------------------------------------------------------------------------
# CMAKE Install files
#---------------------------------------------------------------------------

SET(TINKERCELL_BIN_DIR ${TINKERCELL_BINARY_DIR}/bin)

FILE(GLOB cfiles "${TINKERCELL_SOURCE_DIR}/c/*.h" "${TINKERCELL_SOURCE_DIR}/c/*.c")
INSTALL(FILES ${cfiles} DESTINATION c)

FILE(GLOB cvodeheaders "${TINKERCELL_SOURCE_DIR}/c/cvode/*.h")
INSTALL(FILES ${cvodeheaders} DESTINATION c/cvode)

FILE(GLOB nvectorheaders "${TINKERCELL_SOURCE_DIR}/c/nvector/*.h")
INSTALL(FILES ${nvectorheaders} DESTINATION c/nvector)

FILE(GLOB sundialsheaders "${TINKERCELL_SOURCE_DIR}/c/sundials/*.h")
INSTALL(FILES ${sundialsheaders} DESTINATION c/sundials)

FILE(GLOB cvode_src_cvode "${TINKERCELL_SOURCE_DIR}/c/cvode_src/cvode/*.h" "${TINKERCELL_SOURCE_DIR}/c/cvode_src/cvode/*.c")
INSTALL(FILES ${cvode_src_cvode} DESTINATION c/cvode_src/cvode)

FILE(GLOB cvode_src_cvode_fmix "${TINKERCELL_SOURCE_DIR}/c/cvode_src/cvode/fmix/*.h" "${TINKERCELL_SOURCE_DIR}/c/cvode_src/cvode/fmix/*.c")
INSTALL(FILES ${cvode_src_cvode_fmix} DESTINATION c/cvode_src/cvode/fmix)

FILE(GLOB cvode_src_nvec_par "${TINKERCELL_SOURCE_DIR}/c/cvode_src/nvec_par/*.h" "${TINKERCELL_SOURCE_DIR}/c/cvode_src/nvec_par/*.c")
INSTALL(FILES ${cvode_src_nvec_par} DESTINATION c/cvode_src/nvec_par)

FILE(GLOB cvode_src_sundials "${TINKERCELL_SOURCE_DIR}/c/cvode_src/sundials/*.h" "${TINKERCELL_SOURCE_DIR}/c/cvode_src/sundials/*.c")
INSTALL(FILES ${cvode_src_sundials} DESTINATION c/cvode_src/sundials)

INSTALL(FILES "${TINKERCELL_BIN_DIR}/DB/Regulon/*.*" DESTINATION DB/Regulon)
INSTALL(FILES "${TINKERCELL_BIN_DIR}/NodeItems/*.*" DESTINATION NodeItems)
INSTALL(FILES "${TINKERCELL_BIN_DIR}/ArrowItems/*.*" DESTINATION ArrowItems)
INSTALL(FILES "${TINKERCELL_BIN_DIR}/OtherItems/*.*" DESTINATION OtherItems)
INSTALL(FILES "${TINKERCELL_BIN_DIR}/NodesTree/*.*" DESTINATION NodesTree)

INSTALL(FILES "${TINKERCELL_BIN_DIR}/py/*.*" DESTINATION py)

INSTALL(FILES "${TINKERCELL_BIN_DIR}/Plugins/*.*" DESTINATION Plugins)
INSTALL(FILES "${TINKERCELL_BIN_DIR}/Plugins/c/*.*" DESTINATION Plugins/c)
INSTALL(FILES "${TINKERCELL_BIN_DIR}/*.*" 
  DESTINATION .)

IF(WIN32 AND NOT UNIX)
   INSTALL(FILES "${TINKERCELL_BIN_DIR}/win32/*.*" DESTINATION win32)
   INSTALL(FILES "${TINKERCELL_BIN_DIR}/win32/include/*.*" DESTINATION win32/include)
   INSTALL(FILES "${TINKERCELL_BIN_DIR}/bin/win32/lib/*.*" DESTINATION win32/lib)
ENDIF(WIN32 AND NOT UNIX)

#INSTALL(TARGETS Tinkercell.exe RUNTIME DESTINATION .)

#INSTALL(TARGETS NodeGraphics.exe RUNTIME DESTINATION .)


# ---------------------
# CPACK
# --------------------

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_FILE_NAME "TinkerCell")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "TinkerCell")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "TinkerCell")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "TinkerCell Installer")
SET(CPACK_PACKAGE_VENDOR "Deepak Chandran")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYRIGHT.TXT")
SET(CPACK_PACKAGE_VERSION_MAJOR ${TINKERCELL_MAJOR_VERSION})
SET(CPACK_PACKAGE_VERSION_MINOR ${TINKERCELL_MINOR_VERSION})
IF(WIN32 AND NOT UNIX)
  SET(CPACK_CMAKE_GENERATOR NSIS)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_NSIS_MUI_ICON "${TINKERCELL_SOURCE_DIR}/Main\\\\tinkercell.ico")
  SET(CPACK_PACKAGE_ICON "${TINKERCELL_SOURCE_DIR}/Main\\\\images\\\\tinkercell.png")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\TinkerCellSetup.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${TINKERCELL_INSTALL_DIRECTORY} TinkerCell")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.tinkercell.com")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.tinkercell.com")
  SET(CPACK_NSIS_CONTACT "admin@tinkercell.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "Tinkercell" "Tinkercell")
SET(CPACK_PACKAGE_EXECUTABLES "NodeGraphics" "NodeGraphics")

INCLUDE(CPack)

# ---------------------
# CTEST 
# INCLUDE( CTest )
# --------------------


ENDIF( QT4_FOUND )
