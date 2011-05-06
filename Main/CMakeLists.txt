#------------------------
#  configure code
#------------------------

CONFIGURE_FILE( 
	${TINKERCELL_SOURCE_DIR}/BasicTools/TinkerCellAboutBox.cpp.in
	${TINKERCELL_BINARY_DIR}/BasicTools/TinkerCellAboutBox.cpp
	@ONLY
)

#------------------
# include dirs
#------------------

INCLUDE_DIRECTORIES( BEFORE
    ${TINKERCELL_SOURCE_DIR}/BasicTools
    ${TINKERCELL_SOURCE_DIR}/NodesTree
    ${TINKERCELL_SOURCE_DIR}/Main
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

ADD_DEFINITIONS(-DTINKERCELL_CPP_PLUGINS_FOLDER="${TINKERCELL_CPP_PLUGINS_FOLDER}")
ADD_DEFINITIONS(-DTINKERCELL_C_PLUGINS_FOLDER="${TINKERCELL_C_PLUGINS_FOLDER}")

OPTION( TINKERCELL_BINARY_UPLOAD "Upload TinkerCell binaries using sftp" ON )

#--------------------------------------------------
#  specify IStool and WinSCP for Windows 
#--------------------------------------------------
 
IF (WIN32)
	OPTION( TINKERCELL_INSTALLER "Create installer using Inno setup -- must have Inno setup installed" ON )
	
	IF ((NOT DEFINED INNO) AND TINKERCELL_INSTALLER)
		MESSAGE( "Please create a New Entry of type FILEPATH called INNO and point to the Inno Setup exe file")
	ENDIF ((NOT DEFINED INNO) AND TINKERCELL_INSTALLER)
	
	IF ((NOT DEFINED WINSCP) AND TINKERCELL_BINARY_UPLOAD)
		MESSAGE( "Please create a New Entry of type FILEPATH called WINSCP and point to the WinSCP.exe file")
	ENDIF ((NOT DEFINED WINSCP) AND TINKERCELL_BINARY_UPLOAD)
	
	IF (NOT TINKERCELL_INSTALLER)
		SET (ISTOOL "echo" CACHE STRING "not using Inno setup" FORCE)
		SET (WINSCP "echo" CACHE STRING "not using WinScp" FORCE)
	ENDIF (NOT TINKERCELL_INSTALLER)
	
	IF (NOT TINKERCELL_BINARY_UPLOAD)
		SET (WINSCP "echo" CACHE STRING "not using WinScp" FORCE)
	ENDIF (NOT TINKERCELL_BINARY_UPLOAD)
	
ENDIF(WIN32)

#--------------------------------------------------
#  get sourceforge password for uploading
#--------------------------------------------------

IF (TINKERCELL_BINARY_UPLOAD)
	IF (NOT DEFINED TINKERCELL_SOURCEFORGE_PASSWORD OR NOT DEFINED TINKERCELL_SOURCEFORGE_USERNAME)	
		SET(TINKERCELL_SOURCEFORGE_USERNAME "UNKNOWN" CACHE STRING "your sourceforge username")
		SET(TINKERCELL_SOURCEFORGE_PASSWORD "UNKNOWN" CACHE STRING "your sourceforge password")
	ENDIF (NOT DEFINED TINKERCELL_SOURCEFORGE_PASSWORD OR NOT DEFINED TINKERCELL_SOURCEFORGE_USERNAME)
	
	IF (${TINKERCELL_SOURCEFORGE_USERNAME} MATCHES "UNKNOWN" OR ${TINKERCELL_SOURCEFORGE_PASSWORD} MATCHES "UNKNOWN")
		MESSAGE("If you want to upload to Sourceforge, you must set the values for TINKERCELL_SOURCEFORGE_USERNAME and TINKERCELL_SOURCEFORGE_PASSWORD")
	ENDIF (${TINKERCELL_SOURCEFORGE_USERNAME} MATCHES "UNKNOWN" OR ${TINKERCELL_SOURCEFORGE_PASSWORD} MATCHES "UNKNOWN")
ENDIF (TINKERCELL_BINARY_UPLOAD)

#-----------------------
# TinkerCell variants
#------------------------

OPTION( TINKERCELL_LITE_PARTS "Enable part-only version" OFF )
OPTION( TINKERCELL_LITE_TEXT "Enable text-only version" OFF )

IF( HAVE_64_BIT )
    ADD_DEFINITIONS(-DHAVE_64_BIT)
ENDIF( HAVE_64_BIT )

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
#  Plugins
#--------------------------------------------------------------------------

#INCLUDE (TinkerCellPlugins.cmake) #commented out if plugins are statically loaded

#-----------------------------------
#  Parts catalog
#-----------------------------------

FILE(GLOB NODESTREE_XML_FILES 
	${TINKERCELL_SOURCE_DIR}/NodesTree/*.xml
	${TINKERCELL_SOURCE_DIR}/NodesTree/*.txt
	${TINKERCELL_SOURCE_DIR}/NodesTree/*.nt
)

INSTALL( FILES ${NODESTREE_XML_FILES} DESTINATION NodesTree )

#--------------------------------------------------------------------------
#  Copy example files and stylesheet
#--------------------------------------------------------------------------

FILE(GLOB TINKERCELL_EXAMPLES examples/*.tic examples/*.TIC examples/*.xml)
INSTALL(FILES ${TINKERCELL_EXAMPLES} DESTINATION examples )
INSTALL(FILES tinkercell.qss DESTINATION bin/../ )

#------------------------------------------------
#  configure the BuildAndUploadAll script
#------------------------------------------------

IF (WIN32)
	 #this does not work in windows for some strange reason
ELSE(WIN32)
	IF (APPLE)
		SET(CREATE_TINKERCELL "source mac/CreateApp.sh")
	ELSE(APPLE)
		SET(CREATE_TINKERCELL "source CreatePackage.sh")
		MESSAGE(STATUS "Source BUILD/CreatePackage.sh to build and upload ${TINKERCELL_EXE} to sourceforge")
	ENDIF(APPLE)
	CONFIGURE_FILE( 
		${TINKERCELL_SOURCE_DIR}/Main/BuildAndUploadAll.in
		${TINKERCELL_BINARY_DIR}/BuildAndUploadAll.sh
		@ONLY
	)
	MESSAGE(STATUS "Source BUILD/BuildAndUploadAll.sh to upload full and lite versions")
ENDIF(WIN32)
