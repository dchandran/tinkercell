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

QT4_WRAP_CPP( ModuleTool_MOC ModuleTool.h ModuleConnectionGraphicsItem.h)

ADD_LIBRARY( ModuleTool 
  SHARED
  ModuleTool.cpp
  #ModuleConnectionGraphicsItem.cpp
  ${ModuleTool_MOC}
  ${TinkercellCore_MOC}
  )

TARGET_LINK_LIBRARIES( ModuleTool
  ModuleTool
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


QT4_WRAP_CPP( ModuleIconTool_MOC ModuleIconTool.h)

ADD_LIBRARY( ModuleIconTool 
  SHARED
  ModuleIconTool.cpp
  #ModuleConnectionGraphicsItem.cpp
  ${ModuleIconTool_MOC}
  ${TinkercellCore_MOC}
  )

TARGET_LINK_LIBRARIES( ModuleIconTool
  ModuleIconTool
  TinkerCellCore
)

IF ( WIN32 )
  INSTALL(TARGETS ModuleIconTool
    DESTINATION plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS ModuleIconTool
    LIBRARY DESTINATION plugins
  )
ENDIF( WIN32 )
