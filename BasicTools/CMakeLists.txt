SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

SET( TINKERCELL_CORE_HDRS
  ../Core/MainWindow.h 
  ../Core/NetworkWindow.h
  ../Core/TextEditor.h 
  ../Core/GraphicsScene.h 
  ../Core/Tool.h 
  ../Core/TextItem.h 
  ../Core/ItemHandle.h 
  ../Core/ItemFamily.h
  )

QT4_WRAP_CPP( AboutBox_MOC TinkerCellAboutBox.h ${TINKERCELL_CORE_HDRS})

#CONFIGURE_FILE( TinkerCellAboutBox.cpp
#  ${TINKERCELL_BINARY_DIR}/BasicTools/TinkerCellAboutBox.cpp
#  @ONLY
#)

QT4_ADD_RESOURCES( TINKERCELL_BASICTOOLS_QRC BasicTools.qrc )

ADD_LIBRARY( AboutBox 
  SHARED
  ${AboutBox_MOC}
  TinkerCellAboutBox.cpp 
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

QT4_WRAP_CPP( DnaGraphicsItem_MOC DnaGraphicsItem.h ${TINKERCELL_CORE_HDRS})
QT4_WRAP_CPP( ConnectionMaker_MOC ConnectionMaker.h ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( ConnectionMaker
  SHARED
  ${ConnectionMaker_MOC}
  ${DnaGraphicsItem_MOC}
  ConnectionMaker.cpp
  DnaGraphicsItem.cpp
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( ConnectionMaker
  ConnectionMaker
  TinkerCellCore
)

QT4_WRAP_CPP( ConnectionSelection_MOC ConnectionSelection.h ${TINKERCELL_CORE_HDRS})
QT4_WRAP_CPP( NodeSelection_MOC NodeSelection.h ${TINKERCELL_CORE_HDRS})
QT4_WRAP_CPP( SelectionToolsMain_MOC SelectionToolsMain.h ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( SelectionTools 
  SHARED
  ${ConnectionSelection_MOC}
  ConnectionSelection.cpp
  ${NodeSelection_MOC}
  NodeSelection.cpp
  ${SelectionToolsMain_MOC}
  SelectionToolsMain.cpp
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( SelectionTools
  SelectionTools
  TinkerCellCore
)

QT4_WRAP_CPP( BasicGraphicsToolbox_MOC BasicGraphicsToolbox.h ${TINKERCELL_CORE_HDRS})
QT4_WRAP_CPP( GraphicsTransformTool_MOC GraphicsTransformTool.h ${TINKERCELL_CORE_HDRS})
QT4_WRAP_CPP( TextGraphicsTool_MOC TextGraphicsTool.h ${TINKERCELL_CORE_HDRS})
#QT4_WRAP_CPP( CopyPasteTool_MOC CopyPasteTool.h ${TINKERCELL_CORE_HDRS})
QT4_WRAP_CPP( GroupHandlerTool_MOC GroupHandlerTool.h ${TINKERCELL_CORE_HDRS})
QT4_WRAP_CPP( VisualToolsMain_MOC VisualToolsMain.h ${TINKERCELL_CORE_HDRS})
QT4_WRAP_CPP( GraphicsReplaceTool_MOC GraphicsReplaceTool.h ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( VisualTools 
  SHARED
  ${GraphicsReplaceTool_MOC}
  GraphicsReplaceTool.cpp
  ${BasicGraphicsToolbox_MOC}
  BasicGraphicsToolbox.cpp
  ${GraphicsTransformTool_MOC}
  GraphicsTransformTool.cpp
  ${TextGraphicsTool_MOC}
  TextGraphicsTool.cpp
#  ${CopyPasteTool_MOC}
#  CopyPasteTool.cpp
  ${GroupHandlerTool_MOC}
  GroupHandlerTool.cpp
  ${VisualToolsMain_MOC}
  VisualToolsMain.cpp
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( VisualTools
  VisualTools
  TinkerCellCore 
)

 QT4_WRAP_CPP( SpinBoxDelegate_MOC SpinBoxDelegate.h )
 QT4_WRAP_CPP( DefaultReactionRates_MOC DefaultReactionRates.h ) 
 QT4_WRAP_CPP( BasicInformationTool_MOC BasicInformationTool.h )
 QT4_WRAP_CPP( NameFamilyDialog_MOC NameFamilyDialog.h )
 QT4_WRAP_CPP( StoichiometryTool_MOC StoichiometryTool.h )
 QT4_WRAP_CPP( FunctionDeclarationsTool_MOC FunctionDeclarationsTool.h )
 QT4_WRAP_CPP( ModelSummaryTool_MOC ModelSummaryTool.h )
 QT4_WRAP_CPP( ModelFileGenerator_MOC ModelFileGenerator.h )
 QT4_WRAP_CPP( DataToolsMain_MOC DataToolsMain.h )

ADD_LIBRARY( DataBasicTools
  SHARED
  ${SpinBoxDelegate_MOC}
  ${DefaultReactionRates_MOC}
  ${BasicInformationTool_MOC}
  ${NameFamilyDialog_MOC}
  ${StoichiometryTool_MOC}
  ${FunctionDeclarationsTool_MOC}
  ${ModelSummaryTool_MOC}
  ${ModelFileGenerator_MOC}
  ${DataToolsMain_MOC}
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
