SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

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
  ../Core/Tool.h 
  ../Core/ItemHandle.h 
  ../Core/ItemFamily.h
  )

QT4_WRAP_CPP( TinkercellCore_MOC ${TINKERCELL_CORE_HDRS})

SET(SBML_TOOL_HDRS SBMLImportExport.h)
SET(SBML_TOOL_SRC SBMLImportExport.cpp)

QT4_WRAP_CPP( SBML_TOOL_MOC ${SBML_TOOL_HDRS})

ADD_LIBRARY( SBMLImportExport
  SHARED
  ${SBML_TOOL_MOC}
  ${TinkercellCore_MOC}
  ${SBML_TOOL_SRC}
)

TARGET_LINK_LIBRARIES( SBMLImportExport
  SBMLImportExport
  ModelingTools
  NodesTree
  TinkerCellCore
  muparser
  sbml
  sbml_sim
)

IF( WIN32 )
  INSTALL(TARGETS SBMLImportExport
    DESTINATION plugins
  )
ELSE( WIN32 )
  INSTALL(TARGETS SBMLImportExport
    LIBRARY DESTINATION plugins
  )
ENDIF( WIN32 )

