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

FILE( GLOB ANTIMONYEDITOR_HDRS *.h )
FILE( GLOB ANTIMONYEDITOR_SRC *.cpp )

QT4_WRAP_CPP( ANTIMONYEDITOR_MOC ${ANTYMONYEDITOR_HDRS} ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( AntimonyEditor
  SHARED
  ${ANTIMONYEDITOR_MOC}
  ${ANTIMONYEDITOR_SRC}
 )

TARGET_LINK_LIBRARIES( AntimonyEditor
  AntimonyEditor
  antimony
  DataBasicTools
  TinkerCellCore
)
