FIND_PACKAGE(PythonInterp REQUIRED)

CONFIGURE_FILE( 
	${TINKERCELL_SOURCE_DIR}/win32/CreateInstaller.py
	${TINKERCELL_BINARY_DIR}/win32/CreateInstaller.py
	@ONLY
)

CONFIGURE_FILE( 
	${TINKERCELL_SOURCE_DIR}/win32/CreateInstaller.bat.in
	${TINKERCELL_BINARY_DIR}/win32/CreateInstaller.bat
	@ONLY
)

CONFIGURE_FILE( 
	${TINKERCELL_SOURCE_DIR}/win32/uploadTinkerCell.winscp.in
	${TINKERCELL_BINARY_DIR}/win32/uploadTinkerCell.winscp
	@ONLY
)

MESSAGE(STATUS "Run ${TINKERCELL_BINARY_DIR}/win32/CreateInstaller.bat to build the program, the installer, and upload it to the sourceforge site")
