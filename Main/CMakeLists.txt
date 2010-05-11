SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

QT4_WRAP_CPP( DefaultPluginsMenu_MOC 
  DefaultPluginsMenu.h)

QT4_ADD_RESOURCES( TINKERCELL_QRC ../Core/Tinkercell.qrc )

IF( APPLE AND BUILD_BUNDLE )

  SET(CPACK_BUNDLE_NAME "TinkerCell")  
  SET(CPACK_BUNDLE_PLIST info.plist)  
  SET( MACOSX_BUNDLE_ICON_FILE tinkercell.icns  )
  SET( MACOSX_BUNDLE_SHORT_VERSION_STRING   ${TINKERCELL_VERSION}  )
  SET( MACOSX_BUNDLE_VERSION ${TINKERCELL_VERSION}  )
  SET( MACOSX_BUNDLE_LONG_VERSION_STRING Version ${TINKERCELL_VERSION}  )

  ADD_EXECUTABLE( TinkerCell 
    MACOSX_BUNDLE
    Main.cpp
    ${DefaultPluginsMenu_MOC}
    DefaultPluginsMenu.cpp
    ${TINKERCELL_QRC}
  )

  TARGET_LINK_LIBRARIES( TinkerCell
    TinkerCellCore
    ${QT_LIBRARIES}
  )

  ADD_CUSTOM_COMMAND( TARGET TinkerCell POST_BUILD
      COMMAND mkdir ARGS -p
        ${EXECUTABLE_OUTPUT_PATH}/TinkerCell.app/Contents/Resources
        ${EXECUTABLE_OUTPUT_PATH}/TinkerCell.app/Contents/Frameworks
        ${EXECUTABLE_OUTPUT_PATH}/TinkerCell.app/Contents/MacOS
      COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtCore.framework
        ${EXECUTABLE_OUTPUT_PATH}/TinkerCell.app/Contents/Frameworks/
 	  COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtGui.framework
        ${EXECUTABLE_OUTPUT_PATH}/TinkerCell.app/Contents/Frameworks/
	  COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtXml.framework
        ${EXECUTABLE_OUTPUT_PATH}/TinkerCell.app/Contents/Frameworks/
	  COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtOpenGL.framework
        ${EXECUTABLE_OUTPUT_PATH}/TinkerCell.app/Contents/Frameworks/
      COMMAND cp ARGS ${MACOSX_BUNDLE_ICON_FILE}
        ${EXECUTABLE_OUTPUT_PATH}/TinkerCell.app/Contents/Resources
  )

ELSE( APPLE AND BUILD_BUNDLE )
  IF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
    ADD_EXECUTABLE( TinkerCell 
      Main.cpp
      ${DefaultPluginsMenu_MOC}
      DefaultPluginsMenu.cpp
      ${TINKERCELL_QRC}
    )

    TARGET_LINK_LIBRARIES( TinkerCell
      TinkerCellCore
    )
  ELSE( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
    IF( WIN32 )
		  IF( MINGW )

		  # resource compilation for mingw
  		  ADD_CUSTOM_COMMAND( OUTPUT 
          ${CMAKE_CURRENT_BINARY_DIR}/tinkerell_rc.o
				COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} 
					-i${CMAKE_CURRENT_SOURCE_DIR}/tinkercell.rc
					-o ${CMAKE_CURRENT_BINARY_DIR}/tinkerell_rc.o )		
  			SET( TINKERCELL_RC ${CMAKE_CURRENT_BINARY_DIR}/tinkerell_rc.o )

  		ELSE( MINGW )

	  		SET(TINKERCELL_RC tinkercell.rc)

		  ENDIF( MINGW )
	
    ADD_EXECUTABLE( TinkerCell WIN32
      Main.cpp
      ${DefaultPluginsMenu_MOC}
      DefaultPluginsMenu.cpp
      ${TINKERCELL_QRC}
      ${TINKERCELL_RC}
    )

    TARGET_LINK_LIBRARIES( TinkerCell
      TinkerCellCore
    )
    ENDIF( WIN32 )
  ENDIF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )

ENDIF( APPLE AND BUILD_BUNDLE )

#---------------------------------------------------------------------
#  INSTALL TinkerCell executable and required files and folders
#---------------------------------------------------------------------

INSTALL(TARGETS TinkerCell 
   BUNDLE DESTINATION bin/../
   RUNTIME DESTINATION bin/../
   COMPONENT "TinkerCell")
   
FILE(GLOB TINKERCELL_EXAMPLES examples/*.xml)
INSTALL(FILES ${TINKERCELL_EXAMPLES} DESTINATION examples COMPONENT "examples")
INSTALL(FILES tinkercell.qss DESTINATION bin/../ COMPONENT "stylesheet")
