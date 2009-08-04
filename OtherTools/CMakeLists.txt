SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

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

QT4_WRAP_CPP( ArrowSelection_MOC ArrowSelection.h ${TINKERCELL_CORE_HDRS})

QT4_ADD_RESOURCES( TINKERCELL_OTHERTOOLS_QRC ../Main/Tinkercell.qrc )

ADD_LIBRARY( ArrowSelection 
  SHARED
  ArrowSelection.cpp
  ${ArrowSelection_MOC}
  )

TARGET_LINK_LIBRARIES( ArrowSelection
  ArrowSelection
  TinkerCellCore
)

QT4_WRAP_CPP( AutoGeneRegulatoryTool_MOC AutoGeneRegulatoryTool.h ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( AutoGeneRegulatoryTool
  SHARED
  AutoGeneRegulatoryTool.cpp
  ${AutoGeneRegulatoryTool_MOC}
  )

TARGET_LINK_LIBRARIES( AutoGeneRegulatoryTool
  AutoGeneRegulatoryTool
  TinkerCellCore
)

QT4_WRAP_CPP( DNASequenceTool_MOC DNASequenceTool.h CLabelsTool.h ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( DNASequenceViewer 
  SHARED
  ${DNASequenceTool_MOC}
  CLabelsTool.cpp
  DNASequenceTool.cpp
  )

TARGET_LINK_LIBRARIES( DNASequenceViewer
  DNASequenceViewer
  TinkerCellCore
  AutoGeneRegulatoryTool
)

QT4_WRAP_CPP( ModuleTool_MOC ModuleTool.h ModuleConnectionGraphicsItem.h ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( ModulePlugin 
  SHARED
  ${ModuleTool_MOC}
  ModuleTool.cpp
  ModuleConnectionGraphicsItem.cpp
  )

TARGET_LINK_LIBRARIES( ModulePlugin
  ModulePlugin
  TinkerCellCore
)

QT4_WRAP_CPP( SimulationEventTool_MOC SimulationEventTool.h ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( SimulationEventTool
  SHARED
  ${SimulationEventTool_MOC}
  SimulationEventTool.cpp 
  )

TARGET_LINK_LIBRARIES( SimulationEventTool
  SimulationEventTool
  DataBasicTools
  TinkerCellCore
  muparser
)

QT4_WRAP_CPP( ViewTablesTool_MOC ViewTablesTool.h ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( ViewTables
  SHARED
  ${ViewTablesTool_MOC}
  ViewTablesTool.cpp 
  )

TARGET_LINK_LIBRARIES( ViewTables
  ViewTables
  TinkerCellCore
)

QT4_WRAP_CPP( CellPositionUpdatingTool_MOC CellPositionUpdatingTool.h ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( CellPositionUpdateTool 
  SHARED
  ${CellPositionUpdatingTool_MOC}
  CellPositionUpdatingTool.cpp
  )

TARGET_LINK_LIBRARIES( CellPositionUpdateTool
  CellPositionUpdateTool
  TinkerCellCore 
)

