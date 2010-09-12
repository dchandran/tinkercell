SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

SET( TINKERCELL_CORE_HDRS
  ../Core/MainWindow.h 
  ../Core/NetworkHandle.h
  ../Core/CodeEditor.h 
  ../Core/TextEditor.h 
  ../Core/GraphicsScene.h 
  ../Core/Tool.h
  ../Core/ItemHandle.h 
  ../Core/ItemFamily.h
  )

QT4_WRAP_CPP( TinkercellCore_MOC ${TINKERCELL_CORE_HDRS})

QT4_WRAP_CPP( AutoGeneRegulatoryTool_MOC AutoGeneRegulatoryTool.h)

ADD_LIBRARY( AutoGeneRegulatoryTool
  SHARED
  AutoGeneRegulatoryTool.cpp
  ${AutoGeneRegulatoryTool_MOC}
  ${TinkercellCore_MOC}
  )

TARGET_LINK_LIBRARIES( AutoGeneRegulatoryTool
  AutoGeneRegulatoryTool
  NodesTree
  TinkerCellCore
)

IF ( WIN32 )
  INSTALL(TARGETS AutoGeneRegulatoryTool
    DESTINATION plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS AutoGeneRegulatoryTool
    LIBRARY DESTINATION plugins
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
    DESTINATION plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS DNASequenceViewer
    LIBRARY DESTINATION plugins
  )
ENDIF( WIN32 )

