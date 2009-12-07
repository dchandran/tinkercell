SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

SET( TINKERCELL_CORE_HDRS
  ../Core/MainWindow.h 
  ../Core/NetworkWindow.h
  ../Core/CodeEditor.h 
  ../Core/TextEditor.h 
  ../Core/GraphicsScene.h 
  ../Core/Tool.h 
  ../Core/TextItem.h 
  ../Core/ItemHandle.h 
  ../Core/ItemFamily.h
  )

QT4_WRAP_CPP( TinkercellCore_MOC ${TINKERCELL_CORE_HDRS})

QT4_ADD_RESOURCES( TINKERCELL_BASICTOOLS_QRC ../Main/Tinkercell.qrc )

QT4_WRAP_CPP( ModelingTools_MOC 
   SpinBoxDelegate.h 
   DefaultReactionRates.h 
   BasicInformationTool.h
   StoichiometryTool.h 
   FunctionDeclarationsTool.h
   ModelFileGenerator.h
   ModelSummaryTool.h
   DataToolsMain.h
)

ADD_LIBRARY( ModelingTools
  SHARED
  ${ModelingTools_MOC}
  ${TinkercellCore_MOC}
  EquationParser.cpp
  SpinBoxDelegate.cpp
  BasicInformationTool.cpp
  StoichiometryTool.cpp
  FunctionDeclarationsTool.cpp
  ModelFileGenerator.cpp
  ModelSummaryTool.cpp
  DataToolsMain.cpp
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( ModelingTools
  ModelingTools
  ModuleTool
  TinkerCellCore
  muparser
)

IF( WIN32 )
  INSTALL(TARGETS ModelingTools
    DESTINATION Plugins
  )
ELSE( WIN32 )
  INSTALL(TARGETS ModelingTools
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )

QT4_WRAP_CPP( SimulationEventTool_MOC SimulationEventTool.h)

ADD_LIBRARY( SimulationEventTool
  SHARED
  SimulationEventTool.cpp
  ${SimulationEventTool_MOC}
  ${TinkercellCore_MOC}
  )

TARGET_LINK_LIBRARIES( SimulationEventTool
  SimulationEventTool
  ModelingTools
  ModuleTool
  TinkerCellCore
  muparser
)

IF ( WIN32 )
  INSTALL(TARGETS SimulationEventTool
    DESTINATION Plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS SimulationEventTool
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )
