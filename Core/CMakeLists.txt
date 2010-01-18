SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

SET( TINKERCELL_CORE_HDRS
  AbstractInputWindow.h
  CodeEditor.h
  TextEditor.h
  CThread.h
  GraphicsScene.h
  GraphicsView.h
  HistoryWindow.h
  ItemHandle.h
  MainWindow.h
  NetworkWindow.h
  ConsoleWindow.h
  TextGraphicsItem.h
  TextParser.h
  Tool.h
)

SET( TINKERCELL_CORE_SRC
  AbstractInputWindow.cpp
  CodeEditor.cpp
  ConnectionGraphicsItem.cpp
  ConnectionGraphicsReader.cpp
  ConnectionGraphicsWriter.cpp
  ControlPoint.cpp
  ConvertValue.cpp
  CThread.cpp
  GraphicsScene.cpp
  GraphicsView.cpp
  HistoryWindow.cpp
  ItemFamily.cpp
  ItemHandle.cpp
  MainWindow.cpp
  ModelReader.cpp
  ModelWriter.cpp
  NetworkWindow.cpp
  NodeGraphicsItem.cpp
  NodeGraphicsReader.cpp
  NodeGraphicsWriter.cpp
  ConsoleWindow.cpp
  SymbolsTable.cpp
  TextEditor.cpp
  TextGraphicsItem.cpp
  TextItem.cpp
  TextParser.cpp
  Tool.cpp
  UndoCommands.cpp
  CloneItems.cpp
)

QT4_WRAP_CPP( TINKERCELL_CORE_MOC ${TINKERCELL_CORE_HDRS} )

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

ADD_LIBRARY( TinkerCellCore
  SHARED
  ${TINKERCELL_CORE_MOC}
  ${TINKERCELL_CORE_SRC}
)

TARGET_LINK_LIBRARIES( TinkerCellCore
  TinkerCellCore
  muparser
  ${QT_LIBRARIES}
)

IF( WIN32 )
  INSTALL(TARGETS TinkerCellCore
    DESTINATION bin/../
  )
ELSE( WIN32 )
  INSTALL(TARGETS TinkerCellCore
    LIBRARY DESTINATION bin/../
  )
ENDIF( WIN32 )
