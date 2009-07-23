
QT4_WRAP_CPP( ArrowSelection_MOC ArrowSelection.h )

QT4_ADD_RESOURCES( TINKERCELL_OTHERTOOLS_QRC OtherTools.qrc )

ADD_LIBRARY( ArrowSelection 
  SHARED
  ArrowSelection.cpp
  ${ArrowSelection_MOC}
  )

TARGET_LINK_LIBRARIES( ArrowSelection
  ArrowSelection
  TinkerCellCore
)

QT4_WRAP_CPP( AutoGeneRegulatoryTool_MOC AutoGeneRegulatoryTool.h )

QT4_ADD_RESOURCES( TINKERCELL_OTHERTOOLS_QRC OtherTools.qrc )

ADD_LIBRARY( AutoGeneRegulatoryTool
  SHARED
  AutoGeneRegulatoryTool.cpp
  ${AutoGeneRegulatoryTool_MOC}
  )

TARGET_LINK_LIBRARIES( AutoGeneRegulatoryTool
  AutoGeneRegulatoryTool
  TinkerCellCore
)

QT4_WRAP_CPP( CellPositionUpdatingTool_MOC CellPositionUpdatingTool.h )

QT4_ADD_RESOURCES( TINKERCELL_OTHERTOOLS_QRC OtherTools.qrc )

ADD_LIBRARY( CellPositionUpdateTool 
  SHARED
  ${CellPositionUpdatingTool_MOC}
  CellPositionUpdatingTool.cpp
  )

TARGET_LINK_LIBRARIES( CellPositionUpdateTool
  CellPositionUpdateTool
  TinkerCellCore 
)

QT4_WRAP_CPP( CLabelsTool_MOC CLabelsTool.h )
QT4_WRAP_CPP( DNASequenceTool_MOC DNASequenceTool.h )

QT4_ADD_RESOURCES( TINKERCELL_OTHERTOOLS_QRC OtherTools.qrc )

ADD_LIBRARY( DNASequenceViewer 
  SHARED
  ${CLabelsTool_MOC}
  CLabelsTool.cpp
  ${DNASequenceTool_MOC}
  DNASequenceTool.cpp
  )

TARGET_LINK_LIBRARIES( DNASequenceViewer
  DNASequenceViewer
  TinkerCellCore
  AutoGeneRegulatoryTool
)

QT4_WRAP_CPP( ModuleConnectionGraphicsItem_MOC 
  ModuleConnectionGraphicsItem.h )

QT4_WRAP_CPP( ModuleTool_MOC ModuleTool.h )

QT4_ADD_RESOURCES( TINKERCELL_OTHERTOOLS_QRC OtherTools.qrc )

ADD_LIBRARY( ModulePlugin 
  SHARED
  ${ModuleConnectionGraphicsItem_MOC}
  ${ModuleTool_MOC}
  ModuleTool.cpp
  ModuleConnectionGraphicsItem.cpp
  )

TARGET_LINK_LIBRARIES( ModulePlugin
  ModulePlugin
  TinkerCellCore
)

QT4_WRAP_CPP( SimulationEventTool_MOC SimulationEventTool.h )

QT4_ADD_RESOURCES( TINKERCELL_OTHERTOOLS_QRC OtherTools.qrc )

ADD_LIBRARY( SimulationEventTool
  SHARED
  ${SimulationEventTool_MOC}
  SimulationEventTool.cpp 
  )

TARGET_LINK_LIBRARIES( SimulationEventTool
  SimulationEventTool
  DataBasicTools
  TinkerCellCore
)

QT4_WRAP_CPP( ViewTablesTool_MOC ViewTablesTool.h )

QT4_ADD_RESOURCES( TINKERCELL_OTHERTOOLS_QRC OtherTools.qrc )

ADD_LIBRARY( ViewTables
  SHARED
  ${ViewTablesTool_MOC}
  ViewTablesTool.cpp 
  )

TARGET_LINK_LIBRARIES( ViewTables
  ViewTables
  TinkerCellCore
)

