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

QT4_WRAP_CPP( AboutBox_MOC TinkerCellAboutBox.h ${TINKERCELL_CORE_HDRS})

CONFIGURE_FILE( TinkerCellAboutBox.cpp.in
  ${TINKERCELL_BINARY_DIR}/BasicTools/TinkerCellAboutBox.cpp
  @ONLY
)

QT4_ADD_RESOURCES( TINKERCELL_BASICTOOLS_QRC ../Main/Tinkercell.qrc )

ADD_LIBRARY( AboutBox 
  SHARED
  ${AboutBox_MOC}
  ${TINKERCELL_BINARY_DIR}/BasicTools/TinkerCellAboutBox.cpp 
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( AboutBox
  AboutBox
  TinkerCellCore
)

QT4_WRAP_CPP( InsertionTools_MOC 
  NodeInsertion.h 
  ConnectionInsertion.h 
  CollisionDetection.h
  InsertionToolsMain.h
  ${TINKERCELL_CORE_HDRS}
  )

ADD_LIBRARY( InsertionTools
  SHARED
  ${InsertionTools_MOC}
  NodeInsertion.cpp
  ConnectionInsertion.cpp
  CollisionDetection.cpp
  InsertionToolsMain.cpp
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( InsertionTools
  InsertionTools
  TinkerCellCore
)

QT4_WRAP_CPP( LoadSaveTool_MOC LoadSaveTool.h ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( LoadSaveTool 
  SHARED
  ${LoadSaveTool_MOC}
  LoadSaveTool.cpp 
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( LoadSaveTool
  LoadSaveTool
  TinkerCellCore
)

QT4_WRAP_CPP( ConnectionMaker_MOC 
  DnaGraphicsItem.h 
  ConnectionMaker.h 
  ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( ConnectionMaker
  SHARED
  ${ConnectionMaker_MOC}
  ConnectionMaker.cpp
  DnaGraphicsItem.cpp
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( ConnectionMaker
  ConnectionMaker
  TinkerCellCore
)

QT4_WRAP_CPP( SelectionTools_MOC 
  NodeSelection.h 
  ConnectionSelection.h 
  SelectionToolsMain.h 
  ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( SelectionTools 
  SHARED
  ${SelectionTools_MOC}
  ConnectionSelection.cpp
  NodeSelection.cpp
  SelectionToolsMain.cpp
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( SelectionTools
  SelectionTools
  TinkerCellCore
)

QT4_WRAP_CPP( VisualTools_MOC 
  BasicGraphicsToolbox.h 
  GraphicsTransformTool.h 
  TextGraphicsTool.h
  GroupHandlerTool.h
  VisualToolsMain.h
  GraphicsReplaceTool.h
  ${TINKERCELL_CORE_HDRS}
  )

ADD_LIBRARY( VisualTools 
  SHARED
  ${VisualTools_MOC}
  GraphicsReplaceTool.cpp
  BasicGraphicsToolbox.cpp
  GraphicsTransformTool.cpp
  TextGraphicsTool.cpp
  GroupHandlerTool.cpp
  VisualToolsMain.cpp
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( VisualTools
  VisualTools
  TinkerCellCore 
)

 QT4_WRAP_CPP( DataBasicTools_MOC 
   SpinBoxDelegate.h 
   DefaultReactionRates.h 
   BasicInformationTool.h
   StoichiometryTool.h 
   FunctionDeclarationsTool.h
   ModelSummaryTool.h
   ModelFileGenerator.h
   DataToolsMain.h
   NameFamilyDialog.h
   ${TINKERCELL_CORE_HDRS}
)

ADD_LIBRARY( DataBasicTools
  SHARED
  ${DataBasicTools_MOC}
  SpinBoxDelegate.cpp
  BasicInformationTool.cpp
  NameFamilyDialog.cpp
  StoichiometryTool.cpp
  FunctionDeclarationsTool.cpp
  ModelSummaryTool.cpp
  ModelFileGenerator.cpp
  DataToolsMain.cpp
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( DataBasicTools
  DataBasicTools
  TinkerCellCore
  muparser
)
