CONFIGURE_FILE( 
		${TINKERCELL_SOURCE_DIR}/linux/run_tinkercell.sh
		${TINKERCELL_BINARY_DIR}/linux/run_tinkercell.sh
		@ONLY
	)

INSTALL(FILES ${TINKERCELL_BINARY_DIR}/linux/run_tinkercell.sh DESTINATION bin/../ COMPONENT "TinkerCell")
