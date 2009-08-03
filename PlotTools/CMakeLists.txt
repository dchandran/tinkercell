SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

FILE( GLOB TINKERCELL_PLOT_SRC *.cpp )
FILE( GLOB TINKERCELL_PLOT_HDRS *.h )

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

QT4_WRAP_CPP( TINKERCELL_PLOT_MOC_SRC ${TINKERCELL_PLOT_HDRS} ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( plottool 
  SHARED
  ${TINKERCELL_PLOT_MOC_SRC}
  ${TINKERCELL_PLOT_SRC}
  )

TARGET_LINK_LIBRARIES( plottool
  plottool
  qwt
  TinkerCellCore
)
