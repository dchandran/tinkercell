SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

SET( TINKERCELL_CORE_HDRS
  ../Core/MainWindow.h 
  ../Core/NetworkWindow.h
  ../Core/AbstractInputWindow.h 
  ../Core/CodeEditor.h 
  ../Core/TextEditor.h 
  ../Core/GraphicsScene.h 
  ../Core/Tool.h 
  ../Core/TextItem.h 
  ../Core/ItemHandle.h 
  ../Core/ItemFamily.h
  )

QT4_WRAP_CPP( LPSolveInput_MOC LPSolveInput.h ${TINKERCELL_CORE_HDRS})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

ADD_LIBRARY( LPSolveInput
  SHARED
  LPSolveInput.cpp
  ${LPSolveInput_MOC}
  )

TARGET_LINK_LIBRARIES( LPSolveInput
  LPSolveInput
  ModelingTools
  DynamicCodeTools
  TinkerCellCore
)

IF (WIN32)
   INSTALL(TARGETS LPSolveInput
       DESTINATION Plugins
   )
ELSE(WIN32)
   INSTALL(TARGETS LPSolveInput
       LIBRARY DESTINATION Plugins
   )
ENDIF(WIN32)
