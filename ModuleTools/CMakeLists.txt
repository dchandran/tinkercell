SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})
 INCLUDE_DIRECTORIES( BEFORE
    ${TINKERCELL_SOURCE_DIR}/BasicTools
    ${TINKERCELL_SOURCE_DIR}/NodesTree
 )

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

QT4_WRAP_CPP( ModuleTool_MOC ModuleTool.h )

ADD_LIBRARY( ModuleTool 
  SHARED
  ModuleTool.cpp
  ${ModuleTool_MOC}
  ${TinkercellCore_MOC}
  )

TARGET_LINK_LIBRARIES( ModuleTool
  ModuleTool
  NodesTree
  TinkerCellCore
)

IF ( WIN32 )
  INSTALL(TARGETS ModuleTool
    DESTINATION plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS ModuleTool
    LIBRARY DESTINATION plugins
  )
ENDIF( WIN32 )

