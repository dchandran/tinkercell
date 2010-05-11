SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

CONFIGURE_FILE( TinkerCellAboutBox.cpp.in
  ${TINKERCELL_BINARY_DIR}/BasicTools/TinkerCellAboutBox.cpp
  @ONLY
)

QT4_ADD_RESOURCES( TINKERCELL_BASICTOOLS_QRC ../Core/Tinkercell.qrc )

SET(TINKERCELL_BASICTOOLS_SRC 
  ${TINKERCELL_BINARY_DIR}/BasicTools/TinkerCellAboutBox.cpp
  BasicGraphicsToolbox.cpp
  CollisionDetection.cpp
  ConnectionInsertion.cpp
  ConnectionMaker.cpp
  ConnectionSelection.cpp
  DnaGraphicsItem.cpp
  GraphicsReplaceTool.cpp
  GraphicsTransformTool.cpp
  GroupHandlerTool.cpp
  LoadSaveTool.cpp
  NameFamilyDialog.cpp
  NodeInsertion.cpp
  NodeSelection.cpp
  TextGraphicsTool.cpp
  BasicToolsMain.cpp
)

SET(TINKERCELL_BASICTOOLS_HDR
  BasicGraphicsToolbox.h
  CollisionDetection.h
  ConnectionInsertion.h
  ConnectionMaker.h
  ConnectionSelection.h
  DnaGraphicsItem.h
  GraphicsReplaceTool.h
  GraphicsTransformTool.h
  GroupHandlerTool.h
  LoadSaveTool.h
  NameFamilyDialog.h
  NodeInsertion.h
  NodeSelection.h
  TextGraphicsTool.h
  TinkerCellAboutBox.h
  BasicToolsMain.cpp
)

QT4_WRAP_CPP( BasicTools_MOC ${TINKERCELL_BASICTOOLS_HDR})

ADD_LIBRARY( BasicTools 
  SHARED
  ${BasicTools_MOC}
  ${TinkercellCore_MOC}
  ${TINKERCELL_BASICTOOLS_SRC}
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( BasicTools
  BasicTools
  TinkerCellCore
)

IF( WIN32 )
  INSTALL(TARGETS BasicTools
    DESTINATION plugins
  )
ELSE( WIN32 )
   INSTALL(TARGETS BasicTools
    LIBRARY DESTINATION plugins
  )
ENDIF( WIN32 )
  
  
