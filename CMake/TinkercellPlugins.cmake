#---------------------------------------------------------------------------
# Extend C API to other languages using SWIG (if installed)
#---------------------------------------------------------------------------

ADD_SUBDIRECTORY( python )
ADD_SUBDIRECTORY( ruby )
ADD_SUBDIRECTORY( perl )
ADD_SUBDIRECTORY( octave )
ADD_SUBDIRECTORY( R )
#ADD_SUBDIRECTORY( java )

#---------------------------------------------------------------------------------------
#   TinkerCell plugins  -- all these plugins are now loaded staticall in Main.cpp
#---------------------------------------------------------------------------------------

#The following options allow enabling or disabling of plugins from CMake GUI
#OPTION( PLUGIN_BASIC "Plug-in for enabling basic features" ON )
#OPTION( PLUGIN_MODELING "Plug-in for enabling modeling functions" ON )
#OPTION( PLUGIN_C_PLUGINS "Enable C plug-ins" ON )

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
		INSTALL(DIRECTORY ${TINKERCELL_HOME_DIR}/plugins DESTINATION bin/../)
	ENDIF (NOT DEFINED TINKERCELL_HOME_DIR)
ENDIF (TINKERCELL_3RD_PARTY_PLUGINS)


#copy catalog of parts and connections
FILE(GLOB NODESTREE_XML_FILES 
	${TINKERCELL_SOURCE_DIR}/NodesTree/NodesTree.xml
	${TINKERCELL_SOURCE_DIR}/NodesTree/ConnectionsTree.xml
	${TINKERCELL_SOURCE_DIR}/NodesTree/InitialCatalogList.txt
)

INSTALL( FILES ${NODESTREE_XML_FILES}
  DESTINATION NodesTree
  COMPONENT NodesAndConnections
)

#IF( PLUGIN_BASIC )
#   ADD_SUBDIRECTORY( BasicTools )
#   ADD_SUBDIRECTORY( NodesTree )
#ENDIF( PLUGIN_BASIC )

#IF( PLUGIN_C_PLUGINS )
#   ADD_SUBDIRECTORY( DynamicCodeTools )
   ADD_SUBDIRECTORY( DB )
   ADD_SUBDIRECTORY( c )
#ENDIF( PLUGIN_C_PLUGINS )

#IF( PLUGIN_MODELING )
#  ADD_SUBDIRECTORY( ModelingTools )
#  ADD_SUBDIRECTORY( GeneticNetworks )
#  ADD_SUBDIRECTORY( OtherTools )
#  ADD_SUBDIRECTORY( ImportExportTools )
#ENDIF( PLUGIN_MODELING )

#IF( PLUGIN_MODULES )
#   ADD_SUBDIRECTORY( ModuleTools )
#   ADD_SUBDIRECTORY( Modules )
#   ADD_SUBDIRECTORY( ModularNetworkAlgorithms )
#ENDIF( PLUGIN_MODULES )


