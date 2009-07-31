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

QT4_WRAP_CPP( LPSolveInput_MOC LPSolveInput.h ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( LPSolveInput
  SHARED
  LPSolveInput.cpp
  ${LPSolveInput_MOC}
  )

TARGET_LINK_LIBRARIES( LPSolveInput
  LPSolveInput
  DataBasicTools
  DynamicCodePlugin
  TinkerCellCore
)
