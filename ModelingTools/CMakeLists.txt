SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

  INCLUDE_DIRECTORIES( BEFORE
    ${TINKERCELL_SOURCE_DIR}/BasicTools
    ${TINKERCELL_SOURCE_DIR}/ModelingTools
    ${TINKERCELL_SOURCE_DIR}/NodesTree
 )

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

SET( TINKERCELL_CORE_HDRS
  ../Core/MainWindow.h 
  ../Core/NetworkHandle.h
  ../Core/NetworkWindow.h
  ../Core/CodeEditor.h
  ../Core/TextEditor.h 
  ../Core/GraphicsScene.h
  ../Core/GraphicsView.h  
  ../Core/Tool.h
  )

QT4_WRAP_CPP( TinkercellCore_MOC ${TINKERCELL_CORE_HDRS})

SET(MODELING_TOOLS_HDRS 
  EquationGraph.h
  BasicInformationTool.h
  ModelSummaryTool.h
  ContainerTool.h
  SimulationEventTool.h
  ContainerTreeModel.h
  FunctionDeclarationsTool.h
  SpinBoxDelegate.h
  ModelFileGenerator.h
  StoichiometryTool.h
)

SET(MODELING_TOOLS_SRC 
  EquationGraph.cpp
   BasicInformationTool.cpp
   EquationParser.cpp
   SimulationEventTool.cpp
   ContainerTool.cpp
   FunctionDeclarationsTool.cpp
   SpinBoxDelegate.cpp
   ContainerTreeModel.cpp
   ModelFileGenerator.cpp
   StoichiometryTool.cpp
   DataToolsMain.cpp
   ModelSummaryTool.cpp
)

QT4_WRAP_CPP( ModelingTools_MOC ${MODELING_TOOLS_HDRS})

ADD_LIBRARY( ModelingTools
  SHARED
  ${ModelingTools_MOC}
  ${TinkercellCore_MOC}
  ${MODELING_TOOLS_SRC}
)

TARGET_LINK_LIBRARIES( ModelingTools
  ModelingTools
  NodesTree
  TinkerCellCore
)

IF( WIN32 )
  INSTALL(TARGETS ModelingTools
    DESTINATION plugins
  )
ELSE( WIN32 )
  INSTALL(TARGETS ModelingTools
    LIBRARY DESTINATION plugins
  )
ENDIF( WIN32 )

