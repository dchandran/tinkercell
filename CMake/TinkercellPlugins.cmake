#---------------------------------------------------------------------------
# Extend C API to other languages using SWIG (if installed)
#---------------------------------------------------------------------------

ADD_SUBDIRECTORY( python )
ADD_SUBDIRECTORY( ruby )
ADD_SUBDIRECTORY( perl )
ADD_SUBDIRECTORY( octave )
ADD_SUBDIRECTORY( R )

#---------------------------------------------------------------------------------------
#   TinkerCell plugins  -- all these plugins are now loaded staticall in Main.cpp
#---------------------------------------------------------------------------------------

#The following options allow enabling or disabling of plugins from CMake GUI
#OPTION( PLUGIN_BASIC "Plug-in for enabling basic features" ON )
#OPTION( PLUGIN_MODELING "Plug-in for enabling modeling functions" ON )
#OPTION( PLUGIN_MODULES "Plug-in for enabling modules" ON )
#OPTION( PLUGIN_C_PLUGINS "Enable C plug-ins" ON )

#Compile enables plugins

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


