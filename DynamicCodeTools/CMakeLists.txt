SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

SET( TINKERCELL_CORE_HDRS
  ../Core/MainWindow.h 
  ../Core/NetworkWindow.h
  ../Core/CThread.h 
  ../Core/CodeEditor.h 
  ../Core/TextEditor.h 
  ../Core/GraphicsScene.h 
  ../Core/Tool.h 
  ../Core/TextItem.h 
  ../Core/ItemHandle.h 
  ../Core/ItemFamily.h
  )

FILE( GLOB DYNAMICTOOLS_HDRS *.h )
FILE( GLOB DYNAMICTOOLS_SRC *.cpp )

CONFIGURE_FILE( PythonTool.cpp.in
  ${TINKERCELL_BINARY_DIR}/DynamicTools/PythonTool.cpp
  @ONLY
)

QT4_WRAP_CPP( DYNAMICTOOLS_MOC ${DYNAMICTOOLS_HDRS} ${TINKERCELL_CORE_HDRS})

QT4_ADD_RESOURCES( TINKERCELL_BASICTOOLS_QRC ../Main/Tinkercell.qrc )

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

ADD_LIBRARY( DynamicCodeTools
  SHARED
  ${DYNAMICTOOLS_SRC}
  ${DYNAMICTOOLS_MOC}
  ${TINKERCELL_BINARY_DIR}/DynamicTools/PythonTool.cpp
  )

TARGET_LINK_LIBRARIES( DynamicCodeTools
  DynamicCodeTools
  TinkerCellCore
)
  
