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

FILE( GLOB ANTIMONYEDITOR_HDRS AntimonyEditor.h AntimonySyntaxHighlighter.h )
FILE( GLOB ANTIMONYEDITOR_SRC AntimonyEditor.cpp AntimonySyntaxHighlighter.cpp )
QT4_WRAP_CPP( ANTIMONYEDITOR_MOC ${ANTIMONYEDITOR_HDRS})

  INCLUDE_DIRECTORIES( BEFORE
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/sbml
    ${TINKERCELL_SOURCE_DIR}/ExternalCode/Antimony/src
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbw/include
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbw/include/SBW
	${TINKERCELL_SOURCE_DIR}/ExternalCode/sbw/include/libxml
    ${TINKERCELL_SOURCE_DIR}/c/sbml_sim
    ${TINKERCELL_SOURCE_DIR}/Core
    ${TINKERCELL_SOURCE_DIR}/BasicTools
    ${TINKERCELL_SOURCE_DIR}/ModuleTools
    ${TINKERCELL_SOURCE_DIR}/ModelingTools
    ${TINKERCELL_SOURCE_DIR}/NodesTree
    ${TINKERCELL_SOURCE_DIR}/ImportExportTools
 )

ADD_LIBRARY( AntimonyEditor
  SHARED
  ${ANTIMONYEDITOR_SRC}
  ${ANTIMONYEDITOR_MOC}
  ${TinkercellCore_MOC}
 )

TARGET_LINK_LIBRARIES( AntimonyEditor
  AntimonyEditor
  antimony
  sbml
  sbml_sim
  ModelingTools
  SBMLImportExport
  NodesTree
  TinkerCellCore
)

IF( WIN32 )
  INSTALL(TARGETS AntimonyEditor
    DESTINATION plugins
  )
ELSE( WIN32 )
  INSTALL(TARGETS AntimonyEditor
    LIBRARY DESTINATION plugins
  )
ENDIF( WIN32 )

