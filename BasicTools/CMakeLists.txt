QT4_WRAP_CPP( AboutBox_MOC TinkerCellAboutBox.h )

ADD_LIBRARY( AboutBox 
  SHARED
  ${AboutBox_MOC}
  TinkerCellAboutBox.cpp )

TARGET_LINK_LIBRARIES( AboutBox
  AboutBox
  TinkerCellCore
)

QT4_WRAP_CPP( NodeInsertion_MOC NodeInsertion.h )
QT4_WRAP_CPP( ConnectionInsertion_MOC ConnectionInsertion.h )
QT4_WRAP_CPP( CollisionDetection_MOC CollisionDetection.h )
QT4_WRAP_CPP( InsertionToolsMain_MOC InsertionToolsMain.h )

ADD_LIBRARY( InsertionTools
  SHARED
  ${NodeInsertion_MOC}
  ${ConnectionInsertion_MOC}
  ${CollisionDetection_MOC}
  ${InsertionToolsMain_MOC}
  NodeInsertion.cpp
  ConnectionInsertion.cpp
  CollisionDetection.cpp
  InsertionToolsMain.cpp
  )

TARGET_LINK_LIBRARIES( InsertionTools
  InsertionTools
  TinkerCellCore
)

QT4_WRAP_CPP( LoadSaveTool_MOC LoadSaveTool.h )

ADD_LIBRARY( LoadSaveTool 
  SHARED
  ${LoadSaveTool_MOC}
  LoadSaveTool.cpp 
  )

TARGET_LINK_LIBRARIES( LoadSaveTool
  LoadSaveTool
  TinkerCellCore
)

QT4_WRAP_CPP( DnaGraphicsItem_MOC DnaGraphicsItem.h )
QT4_WRAP_CPP( ConnectionMaker_MOC ConnectionMaker.h )

ADD_LIBRARY( ConnectionMaker
  SHARED
  ${ConnectionMaker_MOC}
  ${DnaGraphicsItem_MOC}
  ConnectionMaker.cpp
  DnaGraphicsItem.cpp
  )

TARGET_LINK_LIBRARIES( ConnectionMaker
  ConnectionMaker
  TinkerCellCore
)

QT4_WRAP_CPP( ConnectionSelection_MOC ConnectionSelection.h )
QT4_WRAP_CPP( NodeSelection_MOC NodeSelection.h )
QT4_WRAP_CPP( SelectionToolsMain_MOC SelectionToolsMain.h )

ADD_LIBRARY( SelectionTools 
  SHARED
  ${ConnectionSelection_MOC}
  ConnectionSelection.cpp
  ${NodeSelection_MOC}
  NodeSelection.cpp
  ${SelectionToolsMain_MOC}
  SelectionToolsMain.cpp
  )

TARGET_LINK_LIBRARIES( SelectionTools
  SelectionTools
  TinkerCellCore
)

QT4_WRAP_CPP( BasicGraphicsToolbox_MOC BasicGraphicsToolbox.h )
QT4_WRAP_CPP( GraphicsTransformTool_MOC GraphicsTransformTool.h )
QT4_WRAP_CPP( TextGraphicsTool_MOC TextGraphicsTool.h )
#QT4_WRAP_CPP( CopyPasteTool_MOC CopyPasteTool.h )
QT4_WRAP_CPP( GroupHandlerTool_MOC GroupHandlerTool.h )
QT4_WRAP_CPP( VisualToolsMain_MOC VisualToolsMain.h )
QT4_WRAP_CPP( GraphicsReplaceTool_MOC GraphicsReplaceTool.h )

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
)

TARGET_LINK_LIBRARIES( VisualTools
  VisualTools
  TinkerCellCore 
)

QT4_WRAP_CPP( TextEditorListener_MOC TextEditorListener.h )
QT4_WRAP_CPP( TextEditorParser_MOC TextEditorParser.h )
QT4_WRAP_CPP( texteditorsyntaxhighlighter_MOC texteditorsyntaxhighlighter.h )

ADD_LIBRARY( TextEditorListener 
  SHARED 
  ${TextEditorListener_MOC} 
  ${texteditorsyntaxhighlighter_MOC} 
  ${TextEditorParser_MOC}
  TextEditorParser.cpp
  TextEditorListener.cpp
  texteditorsyntaxhighlighter.cpp
  )

TARGET_LINK_LIBRARIES( TextEditorListener
  TextEditorListener
  TinkerCellCore
  NodesTree
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
#  DefaultReactionRates.cpp
  BasicInformationTool.cpp
  NameFamilyDialog.cpp
  StoichiometryTool.cpp
  FunctionDeclarationsTool.cpp
  ModelSummaryTool.cpp
  ModelFileGenerator.cpp
  DataToolsMain.cpp
)

TARGET_LINK_LIBRARIES( DataBasicTools
  DataBasicTools
  TinkerCellCore
)
