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

