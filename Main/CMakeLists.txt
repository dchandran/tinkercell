SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

QT4_WRAP_CPP( DefaultPluginsMenu_MOC 
  DefaultPluginsMenu.h 
  ../Core/MainWindow.h )

QT4_ADD_RESOURCES( TINKERCELL_QRC Tinkercell.qrc )

IF( APPLE AND BUILD_BUNDLE )
  SET( MACOSX_TINKERCELL_BUNDLE_ICON_FILE
      ${TINKERCELL_SOURCE_DIR}/Main/tinkercell.icns
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

  ADD_EXECUTABLE( Tinkercell 
    MACOSX_BUNDLE
    Main.cpp
    ${DefaultPluginsMenu_MOC}
    DefaultPluginsMenu.cpp
    ${TINKERCELL_QRC}
  )

  TARGET_LINK_LIBRARIES( Tinkercell
    TinkerCellCore
  )

  ADD_CUSTOM_COMMAND( TARGET Tinkercell POST_BUILD
      COMMAND mkdir ARGS -p
        ${EXECUTABLE_OUTPUT_PATH}/Tinkercell.app/Contents/Resources
        ${EXECUTABLE_OUTPUT_PATH}/Tinkercell.app/Contents/Frameworks
        ${EXECUTABLE_OUTPUT_PATH}/Tinkercell.app/Contents/MacOS
      COMMAND cp ARGS -rf ${QT_LIBRARY_DIR}/QtXml.framework
        ${EXECUTABLE_OUTPUT_PATH}/Tinkercell.app/Contents/Frameworks/
      COMMAND cp ARGS ${EXECUTABLE_OUTPUT_PATH}/libTinkerCellCore.dylib
         ${EXECUTABLE_OUTPUT_PATH}/Tinkercell.app/Contents/MacOS/
      COMMAND cp ARGS ${MACOSX_TINKERCELL_BUNDLE_ICON_FILE}
        ${EXECUTABLE_OUTPUT_PATH}/Tinkercell.app/Contents/Resources
      COMMAND cp ARGS -rf ${EXECUTABLE_OUTPUT_PATH}/Plugins
        ${EXECUTABLE_OUTPUT_PATH}/Tinkercell.app/Contents/MacOS/
  )

ELSE( APPLE AND BUILD_BUNDLE )
  IF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
    ADD_EXECUTABLE( Tinkercell 
      Main.cpp
      ${DefaultPluginsMenu_MOC}
      DefaultPluginsMenu.cpp
      ${TINKERCELL_QRC}
    )

    TARGET_LINK_LIBRARIES( Tinkercell
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
	
    ADD_EXECUTABLE( Tinkercell WIN32 
      Main.cpp
      ${DefaultPluginsMenu_MOC}
      DefaultPluginsMenu.cpp
      ${TINKERCELL_QRC}
      ${TINKERCELL_RC}
    )

    TARGET_LINK_LIBRARIES( Tinkercell
      TinkerCellCore
    )
    ENDIF( WIN32 )
  ENDIF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )

ENDIF( APPLE AND BUILD_BUNDLE )

#---------------------------------------------------------------------
#  INSTALL Tinkercell executable and required files and folders
#---------------------------------------------------------------------

INSTALL(TARGETS Tinkercell 
   RUNTIME DESTINATION .
   COMPONENT "TinkerCell")

INSTALL(FILES tinkercell.qss DESTINATION ./ COMPONENT "stylesheet")