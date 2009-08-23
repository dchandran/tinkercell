#---------------------------------------------------------------------------
# CMAKE Install files
#---------------------------------------------------------------------------

## NOTE: (arnaudgelas)
# There is a special keyword for libraries...
INSTALL(FILES "${TINKERCELL_BINARY_BIN_DIR}/*.*" 
  DESTINATION .
)

IF(WIN32 AND NOT UNIX)
   INSTALL(FILES "${TINKERCELL_BINARY_BIN_DIR}/win32/*.*" DESTINATION win32)
   INSTALL(FILES "${TINKERCELL_BINARY_BIN_DIR}/win32/include/*.*" DESTINATION win32/include)
   INSTALL(FILES "${TINKERCELL_BINARY_BIN_DIR}/win32/lib/*.*" DESTINATION win32/lib)
ENDIF(WIN32 AND NOT UNIX)


# ---------------------
# CPACK
# --------------------

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_FILE_NAME "TinkerCell")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "TinkerCell")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "TinkerCell")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "TinkerCell Installer")
SET(CPACK_PACKAGE_VENDOR "Deepak Chandran")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYRIGHT.TXT")
SET(CPACK_PACKAGE_VERSION_MAJOR ${TINKERCELL_MAJOR_VERSION})
SET(CPACK_PACKAGE_VERSION_MINOR ${TINKERCELL_MINOR_VERSION})
IF(WIN32 AND NOT UNIX)
  #SET(CPACK_CMAKE_GENERATOR NSIS)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_NSIS_MUI_ICON "${TINKERCELL_SOURCE_DIR}/Main\\\\tinkercell.ico")
  SET(CPACK_PACKAGE_ICON "${TINKERCELL_SOURCE_DIR}/Main\\\\images\\\\tinkercell.png")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\TinkerCellSetup.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${TINKERCELL_INSTALL_DIRECTORY} TinkerCell")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.tinkercell.com")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.tinkercell.com")
  SET(CPACK_NSIS_CONTACT "admin@tinkercell.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ENDIF(WIN32 AND NOT UNIX)

SET(CPACK_PACKAGE_EXECUTABLES "Tinkercell" "Tinkercell")
SET(CPACK_PACKAGE_EXECUTABLES "NodeGraphics" "NodeGraphics")

INCLUDE(CPack)

