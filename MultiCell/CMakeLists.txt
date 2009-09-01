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

FILE( GLOB MULTICELL_SRC *.cpp )
FILE( GLOB MULTICELL_HDRS *.h )

QT4_WRAP_CPP( MULTICELL_MOC 
  ${MULTICELL_HDRS}
  ${TINKERCELL_CORE_HDRS} )

QT4_ADD_RESOURCES( TINKERCELL_QRC multicell.qrc )

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

  ADD_EXECUTABLE( MultiCell
    MACOSX_BUNDLE
    ${MULTICELL_SRC}
    ${MULTICELL_MOC}
    ${TINKERCELL_QRC}
  )

  TARGET_LINK_LIBRARIES( MultiCell
    TinkerCellCore
  )
  
  INSTALL(TARGETS MultiCell RUNTIME DESTINATION bin/../)
  
  INSTALL(TARGETS MultiCell RUNTIME DESTINATION bin/../)

  ADD_CUSTOM_COMMAND( TARGET MultiCell POST_BUILD
      COMMAND mkdir ARGS -p
        ${EXECUTABLE_OUTPUT_PATH}/MultiCell.app/Contents/Resources
        ${EXECUTABLE_OUTPUT_PATH}/MultiCell.app/Contents/Frameworks
      COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtXml.framework
        ${EXECUTABLE_OUTPUT_PATH}/MultiCell.app/Contents/Frameworks/
      COMMAND cp ARGS ${EXECUTABLE_OUTPUT_PATH}/libTinkerCellCore.dylib
         ${EXECUTABLE_OUTPUT_PATH}/MultiCell.app/Contents/MacOS/
#      COMMAND cp ARGS ${MACOSX_TINKERCELL_BUNDLE_ICON_FILE}
#        ${EXECUTABLE_OUTPUT_PATH}/MultiCell.app/Contents/Resources
      COMMAND cp ARGS -rf ${LIBRARY_OUTPUT_PATH}
        ${EXECUTABLE_OUTPUT_PATH}/MultiCell.app/Contents/MacOS/
#      COMMAND cp ARGS *.qm
#        ${EXECUTABLE_OUTPUT_PATH}/MultiCell.app/Contents/Resources 
  )

ELSE( APPLE AND BUILD_BUNDLE )
  IF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
    ADD_EXECUTABLE( MultiCell
      ${MULTICELL_SRC}
      ${MULTICELL_MOC}
      ${MULTICELL_QRC}
    )

    TARGET_LINK_LIBRARIES( MultiCell
      TinkerCellCore
    )
  ELSE( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
    IF( WIN32 )
	
	    ADD_EXECUTABLE( MultiCell WIN32 
        ${MULTICELL_SRC}
        ${MULTICELL_MOC}
        ${TINKERCELL_QRC}
       )

      TARGET_LINK_LIBRARIES( MultiCell
        TinkerCellCore
      )
    ENDIF( WIN32 )
  ENDIF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )

ENDIF( APPLE AND BUILD_BUNDLE )
