SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

  INCLUDE_DIRECTORIES( BEFORE
    ${TINKERCELL_SOURCE_DIR}/BasicTools
    ${TINKERCELL_SOURCE_DIR}/OtherTools
    ${TINKERCELL_SOURCE_DIR}/NodesTree
    ${TINKERCELL_SOURCE_DIR}/ModelingTools
    ${TINKERCELL_SOURCE_DIR}/DynamicCodeTools
 )

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

SET( TINKERCELL_CORE_HDRS
  ../Core/MainWindow.h 
  ../Core/NetworkHandle.h
  ../Core/AbstractInputWindow.h 
  ../Core/CodeEditor.h 
  ../Core/TextEditor.h 
  ../Core/GraphicsScene.h 
  ../Core/Tool.h
  ../Core/ItemHandle.h 
  ../Core/ItemFamily.h
  )

QT4_WRAP_CPP( TinkercellCore_MOC ${TINKERCELL_CORE_HDRS})

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
    DESTINATION plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS ViewTablesTool
    LIBRARY DESTINATION plugins
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
    DESTINATION plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS CellPositionUpdateTool
    LIBRARY DESTINATION plugins
  )
ENDIF( WIN32 )

QT4_WRAP_CPP( LPSolveInput_MOC LPSolveInput.h ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( LPSolveInput
  SHARED
  LPSolveInput.cpp
  ${LPSolveInput_MOC}
  ${TinkercellCore_MOC}
)

TARGET_LINK_LIBRARIES( LPSolveInput
  LPSolveInput
  ModelingTools
  DynamicCodeTools
  TinkerCellCore
)

IF (WIN32)
   INSTALL(TARGETS LPSolveInput
       DESTINATION plugins
   )
ELSE(WIN32)
   INSTALL(TARGETS LPSolveInput
       LIBRARY DESTINATION plugins
   )
ENDIF(WIN32)

