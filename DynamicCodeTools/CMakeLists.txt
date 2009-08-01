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
  ../Core/CodeEditor.h
  )

FILE( GLOB DYNAMICTOOLS_HDRS *.h )
FILE( GLOB DYNAMICTOOLS_SRC *.cpp )

QT4_WRAP_CPP( DYNAMICTOOLS_MOC ${DYNAMICTOOLS_HDRS} ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( DynamicCodePlugin
  SHARED
  ${DYNAMICTOOLS_SRC}
  ${DYNAMICTOOLS_MOC}
  )

TARGET_LINK_LIBRARIES( DynamicCodePlugin
  DynamicCodePlugin
  TinkerCellCore
)
  
