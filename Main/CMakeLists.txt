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

#------------------------------------------------
#  configure the BuildAndUploadAll script
#------------------------------------------------

IF (WIN32)
	SET(CREATE_TINKERCELL win32\\CreateInstaller.bat)
	CONFIGURE_FILE( 
		${TINKERCELL_SOURCE_DIR}/Main/BuildAndUploadAll.in
		${TINKERCELL_BINARY_DIR}/BuildAndUploadAll.bat
		@ONLY
	)
	MESSAGE(STATUS "Run the BUILD/BuildAndUpload.bat to automate the entire uploading process.")
ELSE(WIN32)
	IF (APPLE)
		SET(CREATE_TINKERCELL mac/CreateApp.sh)
	ELSE(APPLE)
		SET(CREATE_TINKERCELL CreatePackage.sh)
	ENDIF(APPLE)
	CONFIGURE_FILE( 
		${TINKERCELL_SOURCE_DIR}/Main/BuildAndUploadAll.in
		${TINKERCELL_BINARY_DIR}/BuildAndUploadAll.sh
		@ONLY
	)
	MESSAGE(STATUS "Run the BUILD/BuildAndUpload.sh to automate the entire uploading process.")
ENDIF(WIN32)


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

#--------------------------------------------------
#  specify IStool and WinSCP for Windows 
#--------------------------------------------------
 
IF (WIN32)
	OPTION( TINKERCELL_INSTALLER "Create installer using ISTool -- must have ISTool installed" OFF )
	OPTION( TINKERCELL_INSTALLER_UPLOAD "Create installer using ISTool and upload it using WinSCP -- must have ISTool and WinSCP" OFF )
	
	IF ((NOT DEFINED ISTOOL) AND TINKERCELL_INSTALLER)
		MESSAGE( "Please create a New Entry of type FILEPATH called ISTOOL and point to the ISTool.exe file")
	ENDIF ((NOT DEFINED ISTOOL) AND TINKERCELL_INSTALLER)
	
	IF ((NOT DEFINED WINSCP) AND TINKERCELL_INSTALLER_UPLOAD)
		MESSAGE( "Please create a New Entry of type FILEPATH called WINSCP and point to the WinSCP.exe file")
	ENDIF ((NOT DEFINED WINSCP) AND TINKERCELL_INSTALLER_UPLOAD)
	
	IF (NOT TINKERCELL_INSTALLER)
		SET (ISTOOL "echo" CACHE STRING "not using Inno setup" FORCE)
		SET (WINSCP "echo" CACHE STRING "not using WinScp" FORCE)
	ENDIF (NOT TINKERCELL_INSTALLER)
	
	IF (NOT TINKERCELL_INSTALLER_UPLOAD)
		SET (WINSCP "echo" CACHE STRING "not using WinScp" FORCE)
	ENDIF (NOT TINKERCELL_INSTALLER_UPLOAD)
ENDIF(WIN32)

#--------------------------------------------------
#  get sourceforge password for uploading
#--------------------------------------------------

IF (NOT DEFINED TINKERCELL_SOURCEFORGE_PASSWORD OR NOT DEFINED TINKERCELL_SOURCEFORGE_USERNAME)

	MESSAGE(STATUS "If you want to enable automatic upload to Sourceforge, you must create two New Entries of type STRING, TINKERCELL_SOURCEFORGE_USERNAME and TINKERCELL_SOURCEFORGE_PASSWORD, and populate then with your Sourceforge username and password")

ENDIF (NOT DEFINED TINKERCELL_SOURCEFORGE_PASSWORD OR NOT DEFINED TINKERCELL_SOURCEFORGE_USERNAME)

#-----------------------
# TinkerCell variants
#------------------------

OPTION( TINKERCELL_LITE_PARTS "Enable part-only version" OFF )
OPTION( TINKERCELL_LITE_TEXT "Enable text-only version" OFF )

IF (NOT TINKERCELL_LITE_PARTS AND NOT TINKERCELL_LITE_TEXT)
	MESSAGE(STATUS "Using TinkerCell normal version")
	INCLUDE (TinkerCell.cmake)
ENDIF (NOT TINKERCELL_LITE_PARTS AND NOT TINKERCELL_LITE_TEXT)

IF (TINKERCELL_LITE_PARTS)
	MESSAGE(STATUS "Using TinkerCell lite version")
	INCLUDE (TinkerCellLite.cmake)
ELSEIF (TINKERCELL_LITE_TEXT)
	MESSAGE(STATUS "Using TinkerCell script version")
	INCLUDE (TinkerCellScript.cmake)
ENDIF (TINKERCELL_LITE_PARTS)

#--------------------------------------------------------------------------
#  Copy example files and stylesheet
#--------------------------------------------------------------------------

FILE(GLOB TINKERCELL_EXAMPLES examples/*.tic examples/*.TIC examples/*.xml)
INSTALL(FILES ${TINKERCELL_EXAMPLES} DESTINATION examples COMPONENT "examples")
INSTALL(FILES tinkercell.qss DESTINATION bin/../ COMPONENT "stylesheet")
