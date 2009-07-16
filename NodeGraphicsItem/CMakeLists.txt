SET( TINKERCELL_NODEGRAPHICS_SRC
  NodeGraphicsScene.cpp
  NodeGraphicsWindow.cpp
)

SET( TINKERCELL_NODEGRAPHICS_NAME
  NodeGraphicsScene
  NodeGraphicsWindow
)

FOREACH( src ${TINKERCELL_NODEGRAPHICS_NAME} )
  QT4_WRAP_CPP( ${src}_MOC_SRC ${src}.h )
ENDFOREACH( src )

SET( TINKERCELL_NODEGRAPHICS_MOC_SRC
  ${NodeGraphicsScene_MOC_SRC}
  ${NodeGraphicsWindow_MOC_SRC}
)

QT4_ADD_RESOURCES( TINKERCELL_NODEGRAPHICS_QRC PartGraphics.qrc )

IF( APPLE AND BUILD_BUNDLE )

  SET( MACOSX_NODEGRAPHICS_BUNDLE_ICON_FILE
       ${TINKERCELL_SOURCE_DIR}/NodeGraphicsItem/nodegraphics.icns
  )

  ADD_EXECUTABLE( NodeGraphics
    MACOSX_BUNDLE
    NodeGraphicsMain.cpp
    ${TINKERCELL_NODEGRAPHICS_MOC_SRC}
    ${TINKERCELL_NODEGRAPHICS_SRC}
    ${TINKERCELL_NODEGRAPHICS_QRC}
  )

  TARGET_LINK_LIBRARIES( NodeGraphics
    TinkerCellCore
    ${QT_LIBRARIES}
    muparser
  )
 
  ADD_CUSTOM_COMMAND( TARGET NodeGraphics POST_BUILD
    COMMAND mkdir ARGS 
      ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/Resources
      ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/Frameworks
    COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtGui.framework
      ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/Frameworks/
    COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtCore.framework
      ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/Frameworks/
    COMMAND cp ARGS ${LIBRARY_OUTPUT_PATH}/libTinkerCellCore.dylib
      ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/MacOS/
    COMMAND cp ARGS ${MACOSX_NODEGRAPHICS_BUNDLE_ICON_FILE}
      ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/Resources
    COMMAND cp ARGS -rf ${LIBRARY_OUTPUT_PATH}
      ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/MacOS/
#    COMMAND cp ARGS *.qm
#      ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/Resources 
  )

ELSE( APPLE AND BUILD_BUNDLE )
  IF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
    ADD_EXECUTABLE( NodeGraphics
      NodeGraphicsMain.cpp
      ${TINKERCELL_NODEGRAPHICS_MOC_SRC}
      ${TINKERCELL_NODEGRAPHICS_SRC}
      ${TINKERCELL_NODEGRAPHICS_QRC}
    )

    TARGET_LINK_LIBRARIES( NodeGraphics
      TinkerCellCore
      ${QT_LIBRARIES}
      muparser
    )

  ENDIF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
ENDIF( APPLE AND BUILD_BUNDLE )
