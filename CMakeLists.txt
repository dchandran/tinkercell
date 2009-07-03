PROJECT( TINKERCELL )

SET( TINKERCELL_MAJOR_VERSION "0" )
SET( TINKERCELL_MINOR_VERSION "7" )

SET( TINKERCELL_VERSION
"${TINKERCELL_MAJOR_VERSION}.${TINKERCELL_MINOR_VERSION}" )

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
# Define EXECUTABLE_OUTPUT_PATH and LIBRARY_OUTPUT_PATH
#---------------------------------------------------------------------------
SET(EXECUTABLE_OUTPUT_PATH
 ${TINKERCELL_BINARY_DIR}/bin
 CACHE PATH "Single output directory for building all executables."
)
SET(LIBRARY_OUTPUT_PATH
 ${TINKERCELL_BINARY_DIR}/bin
 CACHE PATH "Single output directory for building all libraries."
)

FIND_PACKAGE( PythonLibs )

IF( NOT PYTHONLIBS_FOUND )
  MESSAGE( SEND_ERROR "TinkerCell required python to be installed" )
ENDIF( NOT PYTHONLIBS_FOUND )

FIND_PACKAGE( Qt4 )

#--------------------------------------------
# Build only if the version of qt is newer
# than Qt4.4. Note Qt starts being LGPL with
# Qt4.5
# IF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )
IF( QT4_FOUND )
  IF( QT_USE_FILE )
    SET( QT_USE_QTSVG TRUE )
    SET( QT_USE_QTXML TRUE )
    INCLUDE( ${QT_USE_FILE} )
  ELSE( QT_USE_FILE )
    SET( QT_LIBRARIES ${QT_QT_LIBRARY} )
  ENDIF( QT_USE_FILE )

  LINK_DIRECTORIES(
   ${LIBRARY_OUTPUT_PATH}
   ${TINKERCELL_SOURCE_DIR}/ExternalLib/
   )

  INCLUDE_DIRECTORIES( BEFORE
    ${PYTHON_INCLUDE_PATH}
    ${QT_INCLUDES}
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/muparser/
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/qwt/
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
#    ${TINKERCELL_SOURCE_DIR}/OtherTools/
#    ${TINKERCELL_SOURCE_DIR}/OtherTools/AntinomyEditor/
    ${TINKERCELL_SOURCE_DIR}/DataTools/
#    ${TINKERCELL_SOURCE_DIR}/NodeGraphicsItem/
    ${TINKERCELL_SOURCE_DIR}/NodesTree/
#    ${TINKERCELL_SOURCE_DIR}/PlotTools/
#    ${TINKERCELL_SOURCE_DIR}/ContainerTools/
#    ${TINKERCELL_SOURCE_DIR}/SegmentationAnimation/
    ${TINKERCELL_SOURCE_DIR}/CInputWindows/
#    ${TINKERCELL_SOURCE_DIR}/DynamicCodeTools/
### includes in BINARY_DIR just in case
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
#    ${TINKERCELL_BINARY_DIR}/OtherTools/
#    ${TINKERCELL_BINARY_DIR}/OtherTools/AntinomyEditor/
    ${TINKERCELL_BINARY_DIR}/DataTools/
#    ${TINKERCELL_BINARY_DIR}/NodeGraphicsItem/
    ${TINKERCELL_BINARY_DIR}/NodesTree/
#    ${TINKERCELL_BINARY_DIR}/PlotTools/
#    ${TINKERCELL_BINARY_DIR}/ContainerTools/
#    ${TINKERCELL_BINARY_DIR}/SegmentationAnimation/
    ${TINKERCELL_BINARY_DIR}/CInputWindows/
#    ${TINKERCELL_BINARY_DIR}/DynamicCodeTools/
 )

  ADD_DEFINITIONS( -DINLINE=static -DYY_NEVER_INTERACTIVE -DPARSER_LP -DINVERSE_ACTIVE=INVERSE_LUSOL -DRoleIsExternalInvEngine )

  ADD_SUBDIRECTORY( c )
  ADD_SUBDIRECTORY( ExternalCode )
  ADD_SUBDIRECTORY( Core )
#  ADD_SUBDIRECTORY( NodeGraphicsItem )
  ADD_SUBDIRECTORY( NodesTree )
#  ADD_SUBDIRECTORY( PlotTools )
##  ADD_SUBDIRECTORY( DataTools )
  ADD_SUBDIRECTORY( BasicTools )
#  ADD_SUBDIRECTORY( OtherTools )
#  ADD_SUBDIRECTORY( ContainerTools )
#  ADD_SUBDIRECTORY( SegmentationAnimation )
  ADD_SUBDIRECTORY( CInputWindows )
#  ADD_SUBDIRECTORY( DynamicCodeTools )
##  ADD_SUBDIRECTORY( c/lpsolve )

#  QT4_ADD_RESOURCES( TINKERCELL_QRC Tinkercell.qrc )

IF( APPLE )

  SET( MACOSX_BUNDLE_ICON_FILE
    ${TINKERCELL_SOURCE_DIR}/tinkercell.icns
    )

  SET( MACOSX_BUNDLE_SHORT_VERSION_STRING
    ${TINKERCELL_VERSION}
    )

  SET( MACOSX_BUNDLE_VERSION
    ${TINKERCELL_VERSION}
    )

  SET( MACOSX_BUNDLE_LONG_VERSION_STRING
    Version ${TINKERCELL_VERSION}
    )

#  ADD_EXECUTABLE( tinkercell
#    MACOSX_BUNDLE
#    Main.cpp
#    ${TINKERCELL_QRC}
#    )
#
#  IF( EXISTS ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Resources )
#
#    ADD_CUSTOM_COMMAND( TARGET tinkercell POST_BUILD
#      COMMAND cp ARGS ${MACOSX_BUNDLE_ICON_FILE} ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Resources
#      #COMMAND cp ARGS *.qm ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Resources 
#)
#
#  ELSE( EXISTS ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Resources )
#
#     ADD_CUSTOM_COMMAND( TARGET tinkercell POST_BUILD
#      COMMAND mkdir ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Resources
#      COMMAND cp ARGS ${MACOSX_BUNDLE_ICON_FILE} ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Resources
#      #COMMAND cp ARGS *.qm ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Resources 
#)
#  ENDIF( EXISTS ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Resources )
#
ENDIF( APPLE )
#
#  TARGET_LINK_LIBRARIES( tinkercell
#    TinkerCellCore
#    ${QT_LIBRARIES}
#    muparser
#    )
## ENDIF( QT4_FOUND AND QT_VERSION_MINOR GREATER 4 )
ENDIF( QT4_FOUND )
