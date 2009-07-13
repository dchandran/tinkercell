QT4_WRAP_CPP( DefaultPluginsMenu_MOC DefaultPluginsMenu.h )

QT4_ADD_RESOURCES( TINKERCELL_QRC Tinkercell.qrc )

IF( APPLE )
  OPTION( BUILD_BUNDLE "Make a Bundle Application" ON )
ENDIF( APPLE )

IF( APPLE AND BUILD_BUNDLE )
  SET( MACOSX_BUNDLE_ICON_FILE
#      ${GOFIGURE2_SOURCE_DIR}/Code/GUI/resources/myapp.icns
  )
  SET( MACOSX_BUNDLE_SHORT_VERSION_STRING
#      ${GOFIGURE2_VERSION}
  )
  SET( MACOSX_BUNDLE_VERSION
#      ${GOFIGURE2_VERSION}
  )
  SET( MACOSX_BUNDLE_LONG_VERSION_STRING
#      Version ${GOFIGURE2_VERSION}
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
      COMMAND cp ARGS ${MACOSX_BUNDLE_ICON_FILE}
        ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Resources
      COMMAND cp ARGS *.qm
        ${EXECUTABLE_OUTPUT_PATH}/tinkercell.app/Contents/Resources 
  )

ENDIF( APPLE AND BUILD_BUNDLE )

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

