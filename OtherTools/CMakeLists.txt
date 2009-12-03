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

QT4_WRAP_CPP( ArrowSelection_MOC ArrowSelection.h)

QT4_ADD_RESOURCES( TINKERCELL_OTHERTOOLS_QRC ../Main/Tinkercell.qrc )

ADD_LIBRARY( ArrowSelection 
  SHARED
  ArrowSelection.cpp
  ${ArrowSelection_MOC}
  ${TinkercellCore_MOC}
  )

TARGET_LINK_LIBRARIES( ArrowSelection
  ArrowSelection
  TinkerCellCore
)

IF ( WIN32 )
  INSTALL(TARGETS ArrowSelection
    DESTINATION Plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS ArrowSelection
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )

QT4_WRAP_CPP( AutoGeneRegulatoryTool_MOC AutoGeneRegulatoryTool.h)

ADD_LIBRARY( AutoGeneRegulatoryTool
  SHARED
  AutoGeneRegulatoryTool.cpp
  ${AutoGeneRegulatoryTool_MOC}
  ${TinkercellCore_MOC}
  )

TARGET_LINK_LIBRARIES( AutoGeneRegulatoryTool
  AutoGeneRegulatoryTool
  TinkerCellCore
)

IF ( WIN32 )
  INSTALL(TARGETS AutoGeneRegulatoryTool
    DESTINATION Plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS AutoGeneRegulatoryTool
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )


QT4_WRAP_CPP( DNASequenceTool_MOC DNASequenceTool.h CLabelsTool.h)

ADD_LIBRARY( DNASequenceViewer 
  SHARED
  CLabelsTool.cpp
  DNASequenceTool.cpp
  ${DNASequenceTool_MOC}
  ${TinkercellCore_MOC}
  )

TARGET_LINK_LIBRARIES( DNASequenceViewer
  DNASequenceViewer
  TinkerCellCore
  AutoGeneRegulatoryTool
)

IF ( WIN32 )
  INSTALL(TARGETS DNASequenceViewer
    DESTINATION Plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS DNASequenceViewer
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )

QT4_WRAP_CPP( ViewTablesTool_MOC ViewTablesTool.h)

ADD_LIBRARY( ViewTablesTool
  SHARED
  ViewTablesTool.cpp
  ${ViewTablesTool_MOC}
  ${TinkercellCore_MOC}
  )

TARGET_LINK_LIBRARIES( ViewTablesTool
  ViewTablesTool
  TinkerCellCore
)

IF ( WIN32 )
  INSTALL(TARGETS ViewTablesTool
    DESTINATION Plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS ViewTablesTool
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )

QT4_WRAP_CPP( CellPositionUpdatingTool_MOC CellPositionUpdatingTool.h)

ADD_LIBRARY( CellPositionUpdateTool
  SHARED
  CellPositionUpdatingTool.cpp
  ${CellPositionUpdatingTool_MOC}
  ${TinkercellCore_MOC}
  )

TARGET_LINK_LIBRARIES( CellPositionUpdateTool
  CellPositionUpdateTool
  TinkerCellCore 
)

IF ( WIN32 )
  INSTALL(TARGETS CellPositionUpdateTool
    DESTINATION Plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS CellPositionUpdateTool
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )

