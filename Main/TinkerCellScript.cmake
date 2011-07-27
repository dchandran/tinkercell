ADD_DEFINITIONS(-DTINKERCELL_TEXT_ONLY)

#----------------------------------------------
#  Define all the cpp files for the binary
#----------------------------------------------

FILE( GLOB TINKERCELL_MAIN_SRC 
	${TINKERCELL_SOURCE_DIR}/Main/Main.cpp
	${TINKERCELL_SOURCE_DIR}/Main/DefaultPluginsMenu.cpp
    ${TINKERCELL_SOURCE_DIR}/DynamicCodeTools/PythonTool.cpp
	${TINKERCELL_SOURCE_DIR}/DynamicCodeTools/OctaveTool.cpp
	${TINKERCELL_SOURCE_DIR}/ModelingTools/*.cpp
    ${TINKERCELL_SOURCE_DIR}/ModuleTools/*.cpp
    ${TINKERCELL_SOURCE_DIR}/GeneticNetworks/*.cpp
    ${TINKERCELL_SOURCE_DIR}/OtherTools/*.cpp
    ${TINKERCELL_SOURCE_DIR}/ImportExportTools/*.cpp
	${TINKERCELL_SOURCE_DIR}/NodesTree/*.cpp
    ${TINKERCELL_SOURCE_DIR}/BasicTools/*.cpp
    ${TINKERCELL_BINARY_DIR}/BasicTools/*.cpp
#    ${TINKERCELL_SOURCE_DIR}/ModularNetworkAlgorithms/*.cpp
)

FILE( GLOB TINKERCELL_MAIN_HRDS
	${TINKERCELL_SOURCE_DIR}/Main/DefaultPluginsMenu.h
    ${TINKERCELL_SOURCE_DIR}/DynamicCodeTools/PythonTool.h
	${TINKERCELL_SOURCE_DIR}/DynamicCodeTools/OctaveTool.h
	${TINKERCELL_SOURCE_DIR}/ModelingTools/*.h
    ${TINKERCELL_SOURCE_DIR}/ModuleTools/*.h
    ${TINKERCELL_SOURCE_DIR}/GeneticNetworks/*.h
    ${TINKERCELL_SOURCE_DIR}/OtherTools/*.h
    ${TINKERCELL_SOURCE_DIR}/ImportExportTools/*.h
	${TINKERCELL_SOURCE_DIR}/BasicTools/*.h
    ${TINKERCELL_SOURCE_DIR}/NodesTree/*.h
)

QT4_WRAP_CPP( TINKERCELL_MAIN_MOC ${TINKERCELL_MAIN_HRDS})
QT4_ADD_RESOURCES( TINKERCELL_QRC ../Core/Tinkercell.qrc )


#--------------------------------------------
#      Make the binary
#--------------------------------------------


IF( APPLE AND BUILD_BUNDLE )

  SET(CPACK_BUNDLE_NAME "TinkerCell")  
  SET(CPACK_BUNDLE_PLIST info.plist)  
  SET( MACOSX_BUNDLE_ICON_FILE 	${TINKERCELL_SOURCE_DIR}/Main/tinkercell.icns  )
  SET( MACOSX_BUNDLE_SHORT_VERSION_STRING   ${TINKERCELL_VERSION}  )
  SET( MACOSX_BUNDLE_VERSION ${TINKERCELL_VERSION}  )
  SET( MACOSX_BUNDLE_LONG_VERSION_STRING Version ${TINKERCELL_VERSION}  )

  ADD_EXECUTABLE( TinkerCellScript 
    MACOSX_BUNDLE
    ${TINKERCELL_MAIN_SRC}
    ${TINKERCELL_MAIN_MOC}
	${TINKERCELL_CORE_MOC}
    ${TINKERCELL_QRC}
  )

  TARGET_LINK_LIBRARIES( TinkerCellScript
    TinkerCellCore
     muparser
     copasi
     sbml
     antimony
    ${QT_LIBRARIES}
  )

  ADD_CUSTOM_COMMAND( TARGET TinkerCellScript PRE_BUILD
      COMMAND mkdir ARGS -p
        ${EXECUTABLE_OUTPUT_PATH}/TinkerCellScript.app/Contents/Resources
      COMMAND cp ARGS ${MACOSX_BUNDLE_ICON_FILE}
        ${EXECUTABLE_OUTPUT_PATH}/TinkerCellScript.app/Contents/Resources
  )

ELSE( APPLE AND BUILD_BUNDLE )
  IF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
    ADD_EXECUTABLE( TinkerCellScript 
      ${TINKERCELL_MAIN_SRC}
      ${TINKERCELL_MAIN_MOC}
	  ${TINKERCELL_CORE_MOC}
      ${TINKERCELL_QRC}
    )

    TARGET_LINK_LIBRARIES( TinkerCellScript
    TinkerCellCore
     muparser
     copasi
     sbml
     antimony
      ${QT_LIBRARIES}
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
	
    ADD_EXECUTABLE( TinkerCellScript WIN32
      ${TINKERCELL_MAIN_SRC}
      ${TINKERCELL_MAIN_MOC}
	  ${TINKERCELL_CORE_MOC}
      ${TINKERCELL_QRC}
      ${TINKERCELL_RC}
    )

    TARGET_LINK_LIBRARIES( TinkerCellScript
    TinkerCellCore
     muparser
     copasi
     sbml
     antimony
      ${QT_LIBRARIES}
    )
    ENDIF( WIN32 )
  ENDIF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )

ENDIF( APPLE AND BUILD_BUNDLE )

#--------------------------------------------------------------------------
#  INSTALL TinkerCell executable and required files and folders
#--------------------------------------------------------------------------

INSTALL(TARGETS TinkerCellScript 
   BUNDLE DESTINATION bin/../
   RUNTIME DESTINATION bin/../
   )

#---------------------------------------------------------------------
#  Updates.txt -- only done in Unix (just for my convenience)
#---------------------------------------------------------------------

IF (NOT APPLE AND UNIX AND EXISTS $ENV{HOME}/Documents/TinkerCell/updates.txt)
  ADD_CUSTOM_COMMAND( TARGET TinkerCell POST_BUILD
  	  COMMAND echo 'updates.txt changed'
      COMMAND perl -pi -e 's/current version: [0-9\\.]+/current version:   ${TINKERCELL_VERSION}/' $ENV{HOME}/Documents/TinkerCell/updates.txt
)
ENDIF (NOT APPLE AND UNIX AND EXISTS $ENV{HOME}/Documents/TinkerCell/updates.txt)

#-------------------------------------------------------------------------------
#  Run Inno setup and WinSCP for windows  or create bundle for Mac
#-------------------------------------------------------------------------------

SET (TINKERCELL_EXE TinkerCellScript CACHE STRING "TinkerCellScript.exe will be the name of the program"
		FORCE)

IF (TINKERCELL_INSTALLER AND WIN32)
    INCLUDE (${TINKERCELL_SOURCE_DIR}/win32/WindowsUploader.cmake)
ENDIF (TINKERCELL_INSTALLER AND WIN32)

IF(APPLE)
    INCLUDE (${TINKERCELL_SOURCE_DIR}/mac/MacUploader.cmake)
ENDIF(APPLE)

IF(UNIX AND NOT APPLE)
    INCLUDE (${TINKERCELL_SOURCE_DIR}/linux/LinuxUploader.cmake)
ENDIF(UNIX AND NOT APPLE)
