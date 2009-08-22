#---------------------------------------------------------------------------
# CMAKE Install files
#---------------------------------------------------------------------------
#
# NOTE: (arnaudgelas)
# that TINKERCELL_BINARY_BIN_DIR is EXECUTABLE_OUTPUT_PATH
SET(TINKERCELL_BIN_DIR ${TINKERCELL_BINARY_DIR}/bin)


#---------------------------------------------------------------------------
#### NOTE: (arnaudgelas)
#### The following has been moved in c/CMakeLists.txt.
#### The best would be to do it when creating corresponding libraries.
#### I have only move the following FILE( GLOB ... and INSTALL( ...
#### for files in c subdirectories.
####
#FILE(GLOB cfiles "${TINKERCELL_SOURCE_DIR}/c/*.h" "${TINKERCELL_SOURCE_DIR}/c/*.c")
#INSTALL(FILES ${cfiles} DESTINATION c)
#
#FILE(GLOB cvodeheaders "${TINKERCELL_SOURCE_DIR}/c/cvode/*.h")
#INSTALL(FILES ${cvodeheaders} DESTINATION c/cvode)
#
#FILE(GLOB nvectorheaders "${TINKERCELL_SOURCE_DIR}/c/nvector/*.h")
#INSTALL(FILES ${nvectorheaders} DESTINATION c/nvector)
#
#FILE(GLOB sundialsheaders "${TINKERCELL_SOURCE_DIR}/c/sundials/*.h")
#INSTALL(FILES ${sundialsheaders} DESTINATION c/sundials)
#
#FILE(GLOB cvode_src_cvode "${TINKERCELL_SOURCE_DIR}/c/cvode_src/cvode/*.h" "${TINKERCELL_SOURCE_DIR}/c/cvode_src/cvode/*.c")
#INSTALL(FILES ${cvode_src_cvode} DESTINATION c/cvode_src/cvode)
#
#FILE(GLOB cvode_src_cvode_fmix "${TINKERCELL_SOURCE_DIR}/c/cvode_src/cvode/fmix/*.h" "${TINKERCELL_SOURCE_DIR}/c/cvode_src/cvode/fmix/*.c")
#INSTALL(FILES ${cvode_src_cvode_fmix} DESTINATION c/cvode_src/cvode/fmix)
#
#FILE(GLOB cvode_src_nvec_par "${TINKERCELL_SOURCE_DIR}/c/cvode_src/nvec_par/*.h" "${TINKERCELL_SOURCE_DIR}/c/cvode_src/nvec_par/*.c")
#INSTALL(FILES ${cvode_src_nvec_par} DESTINATION c/cvode_src/nvec_par)
#
#FILE(GLOB cvode_src_sundials "${TINKERCELL_SOURCE_DIR}/c/cvode_src/sundials/*.h" "${TINKERCELL_SOURCE_DIR}/c/cvode_src/sundials/*.c")
#INSTALL(FILES ${cvode_src_sundials} DESTINATION c/cvode_src/sundials)
#
#---------------------------------------------------------------------------

INSTALL(FILES "${TINKERCELL_BIN_DIR}/DB/Regulon/*.*" 
  DESTINATION DB/Regulon
  # COMPONENTS ??? #choose here a component
)
INSTALL(FILES "${TINKERCELL_BIN_DIR}/NodeItems/*.*" 
  DESTINATION NodeItems
)
INSTALL(FILES "${TINKERCELL_BIN_DIR}/ArrowItems/*.*" 
  DESTINATION ArrowItems
)
INSTALL(FILES "${TINKERCELL_BIN_DIR}/OtherItems/*.*" 
  DESTINATION OtherItems
)
INSTALL(FILES "${TINKERCELL_BIN_DIR}/NodesTree/*.*" 
  DESTINATION NodesTree
)

INSTALL(FILES "${TINKERCELL_BIN_DIR}/py/*.*" 
  DESTINATION py
)

INSTALL(FILES "${TINKERCELL_BIN_DIR}/Plugins/*.*" 
  DESTINATION Plugins
)
INSTALL(FILES "${TINKERCELL_BIN_DIR}/Plugins/c/*.*" 
  DESTINATION Plugins/c
)

## NOTE: (arnaudgelas)
# There is a special keyword for libraries...
INSTALL(FILES "${TINKERCELL_BIN_DIR}/*.*" 
  DESTINATION .
)

IF(WIN32 AND NOT UNIX)
   INSTALL(FILES "${TINKERCELL_BIN_DIR}/win32/*.*" DESTINATION win32)
   INSTALL(FILES "${TINKERCELL_BIN_DIR}/win32/include/*.*" DESTINATION win32/include)
   INSTALL(FILES "${TINKERCELL_BIN_DIR}/bin/win32/lib/*.*" DESTINATION win32/lib)
ENDIF(WIN32 AND NOT UNIX)

#INSTALL(TARGETS Tinkercell.exe RUNTIME DESTINATION .)

#INSTALL(TARGETS NodeGraphics.exe RUNTIME DESTINATION .)


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
  SET(CPACK_CMAKE_GENERATOR NSIS)
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

