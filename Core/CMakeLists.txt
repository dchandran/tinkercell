SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

SET( TINKERCELL_CORE_HDRS
  AbstractInputWindow.h
  CodeEditor.h
  ConnectionGraphicsItem.h
  ConnectionGraphicsReader.h
  ConnectionGraphicsWriter.h
  ControlPoint.h
  ConvertValue.h
  CThread.h
  DataTable.h
  GraphicsScene.h
  HistoryStack.h
  ItemFamily.h
  ItemHandle.h
  MainWindow.h
  ModelReader.h
  ModelWriter.h
  NetworkWindow.h
  NodeGraphicsItem.h
  NodeGraphicsReader.h
  NodeGraphicsWriter.h
  ConsoleWindow.h
  SymbolsTable.h
  TextEditor.h
  TextGraphicsItem.h
  TextItem.h
  TextParser.h
  Tool.h
  UndoCommands.h
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
  HistoryStack.cpp
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

INSTALL(TARGETS TinkerCellCore
  RUNTIME DESTINATION bin/../
)
