SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})


  INCLUDE_DIRECTORIES( BEFORE
    ${TINKERCELL_SOURCE_DIR}/NodeGraphicsItem
 )

ADD_DEFINITIONS(-DQT_NO_DEBUG)

QT4_ADD_RESOURCES( TINKERCELL_NODEGRAPHICS_QRC nodegraphics.qrc )

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


IF( APPLE AND BUILD_BUNDLE )

  SET(CPACK_BUNDLE_NAME "NodeGraphics")  
  SET(CPACK_BUNDLE_PLIST info.plist)  
  SET( MACOSX_BUNDLE_ICON_FILE nodegraphics.icns  )
  SET( MACOSX_BUNDLE_SHORT_VERSION_STRING ${TINKERCELL_VERSION}  )
  SET( MACOSX_BUNDLE_VERSION ${TINKERCELL_VERSION}  )
  SET( MACOSX_BUNDLE_LONG_VERSION_STRING Version ${TINKERCELL_VERSION}  )

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
  )
 
  ADD_CUSTOM_COMMAND( TARGET NodeGraphics POST_BUILD
      COMMAND mkdir ARGS -p 
        ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/Resources
        ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/Frameworks
        ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/MacOS
      
      COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtCore.framework
        ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/Frameworks/

 	  COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtGui.framework
        ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/Frameworks/

	  COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtXml.framework
        ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/Frameworks/
        
	  COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtOpenGL.framework
        ${EXECUTABLE_OUTPUT_PATH}/NodeGraphics.app/Contents/Frameworks/
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
    )
  ELSE( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
    IF( WIN32 )
	
		if(MINGW)
			# resource compilation for mingw
			ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/nodegraphics_rc.o
				COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} 
					-i${CMAKE_CURRENT_SOURCE_DIR}/nodegraphics.rc
					-o ${CMAKE_CURRENT_BINARY_DIR}/nodegraphics_rc.o)		
			SET(NODEGRAPHICS_RC ${CMAKE_CURRENT_BINARY_DIR}/nodegraphics_rc.o)
		else(MINGW)
			SET(NODEGRAPHICS_RC nodegraphics.rc)
		endif(MINGW)
		
      ADD_EXECUTABLE( NodeGraphics WIN32 
        NodeGraphicsMain.cpp
        ${TINKERCELL_NODEGRAPHICS_MOC_SRC}
        ${TINKERCELL_NODEGRAPHICS_SRC}
        ${TINKERCELL_NODEGRAPHICS_QRC}
		${NODEGRAPHICS_RC}
      )

      TARGET_LINK_LIBRARIES( NodeGraphics
        TinkerCellCore
        ${QT_LIBRARIES}
      )
  
    ENDIF( WIN32 )
  ENDIF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
ENDIF( APPLE AND BUILD_BUNDLE )

INSTALL(TARGETS NodeGraphics 
  BUNDLE DESTINATION bin/../
  RUNTIME DESTINATION bin/../ 
  COMPONENT "Node drawing program"
)
