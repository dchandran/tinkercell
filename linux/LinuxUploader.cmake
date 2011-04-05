OPTION( TINKERCELL_UPLOAD_SOURCE "upload source tarball in upload.sh?" OFF )

#the following is used to generate two different files with 32 or 64 in its name
#see upload.sh
IF (${HAVE_64_BIT})
     SET(TINKERCELL_BIT 64)
ELSE(${HAVE_64_BIT})
     SET(TINKERCELL_BIT 32)
ENDIF(${HAVE_64_BIT})


CONFIGURE_FILE( 
		${TINKERCELL_SOURCE_DIR}/linux/run_tinkercell.sh.in
		${TINKERCELL_BINARY_DIR}/run_tinkercell.sh
		@ONLY
	)

CONFIGURE_FILE( 
		${TINKERCELL_SOURCE_DIR}/linux/CreatePackage.sh.in
		${TINKERCELL_BINARY_DIR}/CreatePackage.sh
		@ONLY
	)

MESSAGE(STATUS "To upload TinkerCell: source ${TINKERCELL_BINARY_DIR}/CreatePackage.sh")
	
MESSAGE(STATUS "To run TinkerCell, source ${TINKERCELL_BINARY_DIR}/run_tinkercell.sh")	

INSTALL(FILES ${TINKERCELL_BINARY_DIR}/run_tinkercell.sh DESTINATION bin/../ COMPONENT "TinkerCell")
