ADD_DEFINITIONS(-DTINKERCELL_PARTS_ONLY)

#------------------------
#  configure code
#------------------------

CONFIGURE_FILE( 
	${TINKERCELL_SOURCE_DIR}/BasicTools/TinkerCellAboutBox.cpp.in
	${TINKERCELL_BINARY_DIR}/BasicTools/TinkerCellAboutBox.cpp
	@ONLY
)

CONFIGURE_FILE( 
	${TINKERCELL_SOURCE_DIR}/DynamicCodeTools/CodingWindow.cpp.in
	${TINKERCELL_BINARY_DIR}/DynamicCodeTools/CodingWindow.cpp
	@ONLY
)

#------------------
# include dirs
#------------------

INCLUDE_DIRECTORIES( BEFORE
    ${TINKERCELL_SOURCE_DIR}/BasicTools
    ${TINKERCELL_SOURCE_DIR}/NodesTree
    ${TINKERCELL_SOURCE_DIR}/Main
    ${TINKERCELL_SOURCE_DIR}/DynamicCodeTools
    ${TINKERCELL_SOURCE_DIR}/ModelingTools
    ${TINKERCELL_SOURCE_DIR}/ModuleTools
    ${TINKERCELL_SOURCE_DIR}/GeneticNetworks
     ${TINKERCELL_SOURCE_DIR}/OtherTools
     ${TINKERCELL_SOURCE_DIR}/ImportExportTools
     ${TINKERCELL_SOURCE_DIR}/ExternalCode/copasi
     ${TINKERCELL_SOURCE_DIR}/ExternalCode/libSBML/include
     ${TINKERCELL_SOURCE_DIR}/ExternalCode/Antimony/src
     ${TINKERCELL_SOURCE_DIR}/ModularNetworkAlgorithms
 )

FILE( GLOB TINKERCELL_MAIN_SRC 
	${TINKERCELL_SOURCE_DIR}/Main/*.cpp
	${TINKERCELL_SOURCE_DIR}/NodesTree/*.cpp
    ${TINKERCELL_SOURCE_DIR}/BasicTools/*.cpp
    ${TINKERCELL_BINARY_DIR}/BasicTools/*.cpp
    ${TINKERCELL_SOURCE_DIR}/DynamicCodeTools/*.cpp
    ${TINKERCELL_BINARY_DIR}/DynamicCodeTools/*.cpp
    ${TINKERCELL_SOURCE_DIR}/ModelingTools/*.cpp
    ${TINKERCELL_SOURCE_DIR}/ModuleTools/*.cpp
    ${TINKERCELL_SOURCE_DIR}/GeneticNetworks/*.cpp
    ${TINKERCELL_SOURCE_DIR}/OtherTools/*.cpp
    ${TINKERCELL_SOURCE_DIR}/ImportExportTools/*.cpp
    ${TINKERCELL_SOURCE_DIR}/ModularNetworkAlgorithms/*.cpp
)

FILE( GLOB TINKERCELL_MAIN_HRDS
	${TINKERCELL_SOURCE_DIR}/Main/*.h
    ${TINKERCELL_SOURCE_DIR}/BasicTools/*.h
    ${TINKERCELL_SOURCE_DIR}/NodesTree/*.h
    ${TINKERCELL_SOURCE_DIR}/DynamicCodeTools/*.h
    ${TINKERCELL_SOURCE_DIR}/ModelingTools/*.h
    ${TINKERCELL_SOURCE_DIR}/ModuleTools/*.h
    ${TINKERCELL_SOURCE_DIR}/GeneticNetworks/*.h
    ${TINKERCELL_SOURCE_DIR}/OtherTools/*.h
    ${TINKERCELL_SOURCE_DIR}/ImportExportTools/*.h
    ${TINKERCELL_SOURCE_DIR}/ModularNetworkAlgorithms/*.h
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

  ADD_EXECUTABLE( TinkerCellLite 
    MACOSX_BUNDLE
    ${TINKERCELL_MAIN_SRC}
    ${TINKERCELL_MAIN_MOC}
    ${TINKERCELL_QRC}
  )

  TARGET_LINK_LIBRARIES( TinkerCellLite
    TinkerCellCore
     muparser
	 sbml
	 antimony
	 sbml_sim
	 copasi
    ${QT_LIBRARIES}
  )

  ADD_CUSTOM_COMMAND( TARGET TinkerCellLite PRE_BUILD
      COMMAND mkdir ARGS -p
        ${EXECUTABLE_OUTPUT_PATH}/TinkerCellLite.app/Contents/Resources
      COMMAND cp ARGS ${MACOSX_BUNDLE_ICON_FILE}
        ${EXECUTABLE_OUTPUT_PATH}/TinkerCellLite.app/Contents/Resources
  )

ELSE( APPLE AND BUILD_BUNDLE )
  IF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )
    ADD_EXECUTABLE( TinkerCellLite 
      ${TINKERCELL_MAIN_SRC}
      ${TINKERCELL_MAIN_MOC}
      ${TINKERCELL_QRC}
    )

    TARGET_LINK_LIBRARIES( TinkerCellLite
      TinkerCellCore
       muparser
	   sbml
	   antimony
  	   sbml_sim
	   copasi
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
	
    ADD_EXECUTABLE( TinkerCellLite WIN32
      ${TINKERCELL_MAIN_SRC}
      ${TINKERCELL_MAIN_MOC}
      ${TINKERCELL_QRC}
      ${TINKERCELL_RC}
    )

    TARGET_LINK_LIBRARIES( TinkerCellLite
      TinkerCellCore
       muparser
	   sbml
	   antimony
	   sbml_sim
	   copasi
      ${QT_LIBRARIES}
    )
    ENDIF( WIN32 )
  ENDIF( UNIX OR ( APPLE AND NOT BUILD_BUNDLE ) )

ENDIF( APPLE AND BUILD_BUNDLE )

#--------------------------------------------------------------------------
#  INSTALL TinkerCell executable and required files and folders
#--------------------------------------------------------------------------

INSTALL(TARGETS TinkerCellLite 
   BUNDLE DESTINATION bin/../
   RUNTIME DESTINATION bin/../
   COMPONENT "TinkerCell")
   
FILE(GLOB TINKERCELL_EXAMPLES examples/*.tic examples/*.TIC examples/*.xml)
INSTALL(FILES ${TINKERCELL_EXAMPLES} DESTINATION examples COMPONENT "examples")
INSTALL(FILES tinkercell.qss DESTINATION bin/../ COMPONENT "stylesheet")

#---------------------------------------------------------------------
#  Updates.txt -- only done in Unix (just for my convenience)
#---------------------------------------------------------------------

IF (NOT APPLE AND UNIX AND EXISTS $ENV{HOME}/Documents/TinkerCell/updates.txt)
  ADD_CUSTOM_COMMAND( TARGET TinkerCell POST_BUILD
  	  COMMAND echo 'updates.txt changed'
      COMMAND perl -pi -e 's/current version: [0-9\\.]+/current version:   ${TINKERCELL_VERSION}/' $ENV{HOME}/Documents/TinkerCell/updates.txt
)
ENDIF (NOT APPLE AND UNIX AND EXISTS $ENV{HOME}/Documents/TinkerCell/updates.txt)

#---------------------------------------------------------------------
#  Run Inno setup and WinSCP for windows
#---------------------------------------------------------------------

IF (TINKERCELL_INSTALLER AND WIN32)
   SET (TINKERCELL_EXE TinkerCellLite)

   CONFIGURE_FILE( 
		${TINKERCELL_SOURCE_DIR}/win32/TINKERCELLSETUP.iss.in
		${TINKERCELL_BINARY_DIR}/win32/TINKERCELLSETUP.iss
		@ONLY
	)
	CONFIGURE_FILE( 
		${TINKERCELL_SOURCE_DIR}/win32/uploadTinkerCell.winscp.in
		${TINKERCELL_BINARY_DIR}/win32/uploadTinkerCell.winscp
		@ONLY
	)
	CONFIGURE_FILE( 
		${TINKERCELL_SOURCE_DIR}/win32/makeWin32Installer.bat.in
		${TINKERCELL_BINARY_DIR}/win32/makeWin32Installer.bat
		@ONLY
	)
	MESSAGE("Run ${TINKERCELL_BINARY_DIR}/win32/makeWin32Installer.bat to build the program, the installer, and upload it to the sourceforge site")
ENDIF (TINKERCELL_INSTALLER AND WIN32)

IF(APPLE)
	SET (TINKERCELL_EXE TinkerCellLite)
	SET(QT_QTCORE Versions/4/QtCore)
	SET(QT_QTGUI Versions/4/QtGui)
	SET(QT_QTXML Versions/4/QtXml)
	SET(QT_QTOPENGL Versions/4/QtOpenGL)

	CONFIGURE_FILE( 
	  ${TINKERCELL_SOURCE_DIR}/mac/create_bundled_app.sh.in
	  ${TINKERCELL_BINARY_DIR}/create_bundled_app.sh
	  @ONLY
	)

	MESSAGE("run ${TINKERCELL_BINARY_DIR}/create_bundle_app.sh to make the app")
ENDIF(APPLE)
