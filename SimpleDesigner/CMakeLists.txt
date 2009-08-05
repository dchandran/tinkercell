SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

SET( TINKERCELL_CORE_HDRS
  ../Core/MainWindow.h 
  ../Core/NetworkWindow.h
  ../Core/CodeEditor.h 
  ../Core/TextEditor.h 
  ../Core/GraphicsScene.h 
  ../Core/Tool.h 
  ../Core/TextItem.h 
  ../Core/ItemHandle.h 
  ../Core/ItemFamily.h
  )

QT4_WRAP_CPP( SIMPLEDESIGNER_MOC 
  SimpleDesigner.h
  ${TINKERCELL_CORE_HDRS} )

QT4_ADD_RESOURCES( SIMPLEDESIGNER_QRC Tinkercell.qrc )

IF( APPLE AND BUILD_BUNDLE )
  
  SET( MACOSX_BUNDLE_SHORT_VERSION_STRING
      ${TINKERCELL_VERSION}
  )
  SET( MACOSX_BUNDLE_VERSION
      ${TINKERCELL_VERSION}
  )
  SET( MACOSX_BUNDLE_LONG_VERSION_STRING
      Version ${TINKERCELL_VERSION}
  )

  ADD_CUSTOM_COMMAND( TARGET SimpleDesigner PRE_BUILD
      COMMAND rm ARGS -rf  
        ${EXECUTABLE_OUTPUT_PATH}/SimpleDesigner.app
  )

  ADD_EXECUTABLE( SimpleDesigner
    MACOSX_BUNDLE
    SimpleDesigner.cpp
    ${SIMPLEDESIGNER_MOC}
    DefaultPluginsMenu.cpp
    ${TINKERCELL_QRC}
  )

  TARGET_LINK_LIBRARIES( SimpleDesigner
    TinkerCellCore
  )

  ADD_CUSTOM_COMMAND( TARGET SimpleDesigner POST_BUILD
      COMMAND mkdir ARGS 
        ${EXECUTABLE_OUTPUT_PATH}/SimpleDesigner.app/Contents/Resources
        ${EXECUTABLE_OUTPUT_PATH}/SimpleDesigner.app/Contents/Frameworks
      COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtXml.framework
        ${EXECUTABLE_OUTPUT_PATH}/SimpleDesigner.app/Contents/Frameworks/
      COMMAND cp ARGS ${LIBRARY_OUTPUT_PATH}/libTinkerCellCore.dylib
         ${EXECUTABLE_OUTPUT_PATH}/SimpleDesigner.app/Contents/MacOS/
#      COMMAND cp ARGS ${MACOSX_TINKERCELL_BUNDLE_ICON_FILE}
#        ${EXECUTABLE_OUTPUT_PATH}/SimpleDesigner.app/Contents/Resources
      COMMAND cp ARGS -rf ${LIBRARY_OUTPUT_PATH}
        ${EXECUTABLE_OUTPUT_PATH}/SimpleDesigner.app/Contents/MacOS/
#      COMMAND cp ARGS *.qm
#        ${EXECUTABLE_OUTPUT_PATH}/SimpleDesigner.app/Contents/Resources 
  )

ELSE( APPLE AND BUILD_BUNDLE )
  IF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
    ADD_EXECUTABLE( SimpleDesigner
      SimpleDesigner.cpp
      ${SIMPLEDESIGNER_MOC}
      ${SIMPLEDESIGNER_QRC}
    )

    TARGET_LINK_LIBRARIES( SimpleDesigner
      TinkerCellCore
    )
  ELSE( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
    IF( WIN32 )
	
	    ADD_EXECUTABLE( SimpleDesigner WIN32 
        SimpleDesigner.cpp
        ${SIMPLEDESIGNER_MOC}
        ${TINKERCELL_QRC}
       )

      TARGET_LINK_LIBRARIES( SimpleDesigner
        TinkerCellCore
      )
    ENDIF( WIN32 )
  ENDIF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )

ENDIF( APPLE AND BUILD_BUNDLE )
