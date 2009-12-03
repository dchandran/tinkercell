SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})


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

QT4_WRAP_CPP( TinkercellCOre_MOC ${TINKERCELL_CORE_HDRS})

QT4_WRAP_CPP( AboutBox_MOC TinkerCellAboutBox.h)

CONFIGURE_FILE( TinkerCellAboutBox.cpp.in
  ${TINKERCELL_BINARY_DIR}/BasicTools/TinkerCellAboutBox.cpp
  @ONLY
)

QT4_ADD_RESOURCES( TINKERCELL_BASICTOOLS_QRC ../Main/Tinkercell.qrc )

ADD_LIBRARY( AboutBox 
  SHARED
  ${AboutBox_MOC}
  ${TinkercellCOre_MOC}
  ${TINKERCELL_BINARY_DIR}/BasicTools/TinkerCellAboutBox.cpp 
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( AboutBox
  AboutBox
  TinkerCellCore
)

IF( WIN32 )
  INSTALL(TARGETS AboutBox
    DESTINATION Plugins
  )
ELSE( WIN32 )
  INSTALL(TARGETS AboutBox
  LIBRARY DESTINATION Plugins
  ARCHIVE DESTINATION Plugins
)
ENDIF( WIN32 )

QT4_WRAP_CPP( InsertionTools_MOC 
  NodeInsertion.h 
  ConnectionInsertion.h 
  CollisionDetection.h
  InsertionToolsMain.h
  )

ADD_LIBRARY( InsertionTools
  SHARED
  ${InsertionTools_MOC}
  ${TinkercellCOre_MOC}
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

IF( WIN32 )
  INSTALL(TARGETS InsertionTools
    DESTINATION Plugins
  )
ELSE( WIN32 )
   INSTALL(TARGETS InsertionTools
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )


QT4_WRAP_CPP( LoadSaveTool_MOC LoadSaveTool.h)

ADD_LIBRARY( LoadSaveTool 
  SHARED
  ${LoadSaveTool_MOC}
  ${TinkercellCOre_MOC}
  LoadSaveTool.cpp 
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( LoadSaveTool
  LoadSaveTool
  TinkerCellCore
)

IF( WIN32 )
  INSTALL(TARGETS LoadSaveTool
    DESTINATION Plugins
  )
ELSE( WIN32 )
  INSTALL(TARGETS LoadSaveTool
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )

QT4_WRAP_CPP( ConnectionMaker_MOC 
  DnaGraphicsItem.h 
  ConnectionMaker.h
  )

ADD_LIBRARY( ConnectionMaker
  SHARED
  ${ConnectionMaker_MOC}
  ${TinkercellCOre_MOC}
  ConnectionMaker.cpp
  DnaGraphicsItem.cpp
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( ConnectionMaker
  ConnectionMaker
  TinkerCellCore
)

IF( WIN32 )
  INSTALL(TARGETS ConnectionMaker
    DESTINATION Plugins
  )
ELSE( WIN32 )
  INSTALL(TARGETS ConnectionMaker
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )

QT4_WRAP_CPP( SelectionTools_MOC 
  NodeSelection.h 
  ConnectionSelection.h 
  SelectionToolsMain.h 
  )

ADD_LIBRARY( SelectionTools 
  SHARED
  ${SelectionTools_MOC}
  ${TinkercellCOre_MOC}
  ConnectionSelection.cpp
  NodeSelection.cpp
  SelectionToolsMain.cpp
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( SelectionTools
  SelectionTools
  TinkerCellCore
)

IF( WIN32 )
  INSTALL(TARGETS SelectionTools
    DESTINATION Plugins
  )
ELSE( WIN32 )
  INSTALL(TARGETS SelectionTools
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )

QT4_WRAP_CPP( VisualTools_MOC 
  BasicGraphicsToolbox.h 
  GraphicsTransformTool.h 
  TextGraphicsTool.h
  GroupHandlerTool.h
  VisualToolsMain.h
  GraphicsReplaceTool.h
  )

ADD_LIBRARY( VisualTools 
  SHARED
  ${VisualTools_MOC}
  ${TinkercellCOre_MOC}
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

IF( WIN32 )
  INSTALL(TARGETS VisualTools
    DESTINATION Plugins
  )
ELSE( WIN32 )
  INSTALL(TARGETS VisualTools
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )

QT4_WRAP_CPP( NameFamilyDialog_MOC
   NameFamilyDialog.h
)

ADD_LIBRARY( NameFamilyDialog
  SHARED
  ${NameFamilyDialog_MOC}
  ${TinkercellCore_MOC}
  NameFamilyDialog.cpp
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( NameFamilyDialog
  NameFamilyDialog
  TinkerCellCore
  muparser
)

IF( WIN32 )
  INSTALL(TARGETS NameFamilyDialog
    DESTINATION Plugins
  )
ELSE( WIN32 )
  INSTALL(TARGETS NameFamilyDialog
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )

