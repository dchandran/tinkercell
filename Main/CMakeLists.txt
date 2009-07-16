QT4_WRAP_CPP( DefaultPluginsMenu_MOC DefaultPluginsMenu.h )

QT4_ADD_RESOURCES( TINKERCELL_QRC Tinkercell.qrc )

IF( APPLE )
  OPTION( BUILD_BUNDLE "Make a Bundle Application" ON )
ENDIF( APPLE )

IF( APPLE AND BUILD_BUNDLE )
  SET( MACOSX_BUNDLE_ICON_FILE
      ${TINKERCELL_SOURCE}/Main/tinkercell.icns
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

  ADD_EXECUTABLE( tinkercell 
    MACOSX_BUNDLE
    Main.cpp
    ${DefaultPluginsMenu_MOC}
    DefaultPluginsMenu.cpp
    ${TINKERCELL_QRC}
  )

  TARGET_LINK_LIBRARIES( tinkercell
    TinkerCellCore
  )

  ADD_CUSTOM_COMMAND( TARGET tinkercell POST_BUILD
      COMMAND mkdir ARGS 
        ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Resources
        ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Frameworks
      COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtXml.framework
        ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Frameworks/
      COMMAND cp ARGS ${EXECUTABLE_OUTPUT_PATH}/libTinkerCellCore.dylib
         ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/MacOS/
      COMMAND cp ARGS ${MACOSX_BUNDLE_ICON_FILE}
        ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Resources
      COMMAND cp ARGS *.qm
        ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Resources 
  )

ELSE( APPLE AND BUILD_BUNDLE )
IF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
  ADD_EXECUTABLE( tinkercell 
    Main.cpp
    ${DefaultPluginsMenu_MOC}
    DefaultPluginsMenu.cpp
    ${TINKERCELL_QRC}
  )

  TARGET_LINK_LIBRARIES( tinkercell
    TinkerCellCore
  )
ENDIF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )

ENDIF( APPLE AND BUILD_BUNDLE )
