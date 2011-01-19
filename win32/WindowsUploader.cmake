CONFIGURE_FILE( 
	${TINKERCELL_SOURCE_DIR}/win32/TINKERCELLSETUP.iss.in
	${TINKERCELL_BINARY_DIR}/win32/TINKERCELLSETUP.iss
	@ONLY
)

CONFIGURE_FILE( 
	${TINKERCELL_SOURCE_DIR}/win32/makeWin32Installer.bat.in
	${TINKERCELL_BINARY_DIR}/win32/makeWin32Installer.bat
	@ONLY
)
	
CONFIGURE_FILE( 
	${TINKERCELL_SOURCE_DIR}/win32/uploadTinkerCell.winscp.in
	${TINKERCELL_BINARY_DIR}/win32/uploadTinkerCell.winscp
	@ONLY
)

MESSAGE("Run ${TINKERCELL_BINARY_DIR}/win32/makeWin32Installer.bat to build the program, the installer, and upload it to the sourceforge site")