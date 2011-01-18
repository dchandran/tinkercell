IF (WIN32)
	OPTION( TINKERCELL_INSTALLER "Create installer using ISTool -- must have ISTool installed" OFF )
	OPTION( TINKERCELL_INSTALLER_UPLOAD "Create installer using ISTool and upload it using WinSCP -- must have ISTool and WinSCP" OFF )
	
	IF ((NOT DEFINED ISTOOL) AND TINKERCELL_INSTALLER)
		MESSAGE( "Please create a New Entry of type FILEPATH called ISTOOL and point to the ISTool.exe file")
	ENDIF ((NOT DEFINED ISTOOL) AND TINKERCELL_INSTALLER)
	
	IF ((NOT DEFINED WINSCP) AND TINKERCELL_INSTALLER_UPLOAD)
		MESSAGE( "Please create a New Entry of type FILEPATH called WINSCP and point to the WinSCP.exe file")
	ENDIF ((NOT DEFINED WINSCP) AND TINKERCELL_INSTALLER_UPLOAD)

ENDIF(WIN32)

#-----------------------
# TinkerCell variants
#------------------------

OPTION( TINKERCELL_LITE_PARTS "Enable part-only version" OFF )
OPTION( TINKERCELL_LITE_TEXT "Enable text-only version" OFF )

IF (NOT TINKERCELL_LITE_PARTS AND NOT TINKERCELL_LITE_TEXT)
	MESSAGE("Using TinkerCell normal version")
	INCLUDE (TinkerCell.cmake)
ENDIF (NOT TINKERCELL_LITE_PARTS AND NOT TINKERCELL_LITE_TEXT)

IF (TINKERCELL_LITE_PARTS)
	MESSAGE("Using TinkerCell lite version")
	INCLUDE (TinkerCellLite.cmake)
ELSEIF (TINKERCELL_LITE_TEXT)
	MESSAGE("Using TinkerCell script version")
	INCLUDE (TinkerCellScript.cmake)
ENDIF (TINKERCELL_LITE_PARTS)

