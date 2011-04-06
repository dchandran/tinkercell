#-----------------------------------------------------------------------------------------------------------
#   Most of theTinkerCell plugins are loaded statically in Main.cpp some (below) are loaded dynamically
#-----------------------------------------------------------------------------------------------------------

SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

FILE( GLOB TINKERCELL_PLUGINS_SRC 
    ${TINKERCELL_SOURCE_DIR}/ModelingTools/*.cpp
    ${TINKERCELL_SOURCE_DIR}/ModuleTools/*.cpp
    ${TINKERCELL_SOURCE_DIR}/GeneticNetworks/*.cpp
    ${TINKERCELL_SOURCE_DIR}/OtherTools/*.cpp
    ${TINKERCELL_SOURCE_DIR}/ImportExportTools/*.cpp
#    ${TINKERCELL_SOURCE_DIR}/ModularNetworkAlgorithms/*.cpp
    ${TINKERCELL_SOURCE_DIR}/Main/Plugins.cpp
)

FILE( GLOB TINKERCELL_PLUGINS_HDRS
    ${TINKERCELL_SOURCE_DIR}/ModelingTools/*.h
    ${TINKERCELL_SOURCE_DIR}/ModuleTools/*.h
    ${TINKERCELL_SOURCE_DIR}/GeneticNetworks/*.h
    ${TINKERCELL_SOURCE_DIR}/OtherTools/*.h
    ${TINKERCELL_SOURCE_DIR}/ImportExportTools/*.h
#    ${TINKERCELL_SOURCE_DIR}/ModularNetworkAlgorithms/*.h
)

QT4_WRAP_CPP( TINKERCELL_PLUGINS_MOC 
  ${TINKERCELL_PLUGINS_HDRS}
 )

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

ADD_LIBRARY( ModelingPlugins 
  SHARED
  ${TINKERCELL_PLUGINS_SRC}
  ${TINKERCELL_PLUGINS_MOC}
)

TARGET_LINK_LIBRARIES( ModelingPlugins
  ModelingPlugins
  TinkerCellCore
  muparser
  copasi
  sbml
  antimony
)

IF ( WIN32 )
  INSTALL(TARGETS ModelingPlugins
    DESTINATION ${TINKERCELL_CPP_PLUGINS_FOLDER}
    COMPONENT plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS ModelingPlugins
    LIBRARY DESTINATION ${TINKERCELL_CPP_PLUGINS_FOLDER}
    COMPONENT plugins
  )
ENDIF( WIN32 )

#-----------------------------------
#   TinkerCell 3rd party plugins
#-----------------------------------

OPTION( TINKERCELL_COPY_MODULES "Copy modules from TinkerCell home folder" ON )
OPTION( TINKERCELL_3RD_PARTY_PLUGINS "Copy plugins in TinkerCell home folder" ON )

#copy modules if enabled
IF (TINKERCELL_COPY_MODULES)
	IF (NOT DEFINED TINKERCELL_HOME_DIR)
		MESSAGE("To copy the existing modules, please add entry for TINKERCELL_HOME_DIR")
	ELSE (NOT DEFINED TINKERCELL_HOME_DIR)
		MESSAGE(STATUS "Will copy ${TINKERCELL_HOME_DIR}/Modules")
		INSTALL(DIRECTORY ${TINKERCELL_HOME_DIR}/Modules DESTINATION bin/../)
	ENDIF (NOT DEFINED TINKERCELL_HOME_DIR)
ENDIF (TINKERCELL_COPY_MODULES)

#copy 3rd party plugins if enabled
IF (TINKERCELL_3RD_PARTY_PLUGINS)
	IF (NOT DEFINED TINKERCELL_HOME_DIR)
		MESSAGE("To copy the 3rd party plugins, please add entry for TINKERCELL_HOME_DIR")
	ELSE (NOT DEFINED TINKERCELL_HOME_DIR)
		MESSAGE(STATUS "Will copy /python /octave and /plugins from ${TINKERCELL_HOME_DIR}")
		INSTALL(DIRECTORY ${TINKERCELL_HOME_DIR}/python DESTINATION bin/../)
		INSTALL(DIRECTORY ${TINKERCELL_HOME_DIR}/octave DESTINATION bin/../)
		#INSTALL(DIRECTORY ${TINKERCELL_HOME_DIR}/plugins DESTINATION bin/../)
	ENDIF (NOT DEFINED TINKERCELL_HOME_DIR)
ENDIF (TINKERCELL_3RD_PARTY_PLUGINS)

#-----------------------------------
#  Parts catalog
#-----------------------------------

FILE(GLOB NODESTREE_XML_FILES 
	${TINKERCELL_SOURCE_DIR}/NodesTree/NodesTree.xml
	${TINKERCELL_SOURCE_DIR}/NodesTree/ConnectionsTree.xml
	${TINKERCELL_SOURCE_DIR}/NodesTree/InitialCatalogList.txt
)

INSTALL( FILES ${NODESTREE_XML_FILES}
  DESTINATION NodesTree
  COMPONENT NodesAndConnections
)

