SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

INCLUDE_DIRECTORIES( BEFORE
      ${TINKERCELL_SOURCE_DIR}/ExternalCode/cluster
      ${TINKERCELL_SOURCE_DIR}/ExternalCode/cluster/src
)

FILE(GLOB TINKERCELL_CORE_HDRS 
	AbstractInputWindow.h
	C_API_Slots.h
	CodeEditor.h
	ConsoleWindow.h
	CThread.h
	plots/GnuplotTool.h
	GraphicsScene.h
	GraphicsView.h
	HistoryWindow.h
	interpreters/InterpreterThread.h
	ItemHandle.h
	MainWindow.h
	plugins/MultithreadedSliderWidget.h
	NetworkHandle.h
	NetworkWindow.h
	interpreters/OctaveInterpreterThread.h
	plots/Plot2DWidget.h
	plots/Plot3DWidget.h
	plots/PlotTextWidget.h
	plots/PlotTool.h
	plots/PlotWidget.h
	interpreters/PythonInterpreterThread.h
	TextEditor.h
	TextGraphicsItem.h
	plugins/TextParser.h
	Tool.h
	plugins/TextGraphicsTool.h
	plugins/BasicGraphicsToolbar.h
	fileIO/LoadSaveTool.h
)

FILE(GLOB TINKERCELL_CORE_SRC *.cpp interpreters/*.cpp plots/*.cpp fileIO/*.cpp plugins/*.cpp)

QT4_ADD_RESOURCES( TINKERCELL_QRC Tinkercell.qrc )

QT4_WRAP_CPP( TINKERCELL_CORE_MOC ${TINKERCELL_CORE_HDRS} )

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

ADD_LIBRARY( TinkerCellCore
  SHARED
  ${TINKERCELL_CORE_MOC}
  ${TINKERCELL_CORE_SRC}
  ${TINKERCELL_SOURCE_DIR}/API/TC_structs.c
  ${TINKERCELL_QRC}
)

TARGET_LINK_LIBRARIES( TinkerCellCore
  TinkerCellCore
  muparser  #for parsing math
  qwt           #for plotting
  qwtplot3d   #for plotting 3D
  cluster      #for clustering plot data
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

