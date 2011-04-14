SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

OPTION( ENABLE_CONSOLE_WINDOW "Enable console window" ON )
OPTION( ENABLE_HISTORY_WINDOW "History stack widget" ON )
OPTION( ENABLE_GRAPHING_TOOLS "2D plot, 3D plot, and gnuplot tools" ON )
OPTION( ENABLE_CODING_TOOLS "Coding window, Functions menu, Python tool, and Octave tool" ON )

IF (ENABLE_HISTORY_WINDOW)
	ADD_DEFINITIONS( -DENABLE_HISTORY_WINDOW )
ENDIF (ENABLE_HISTORY_WINDOW)

IF (ENABLE_GRAPHING_TOOLS)
	ADD_DEFINITIONS( -DENABLE_GRAPHING_TOOLS )
ENDIF (ENABLE_GRAPHING_TOOLS)

IF (ENABLE_CODING_TOOLS)
	ADD_DEFINITIONS( -DENABLE_CODING_TOOLS )
ENDIF (ENABLE_CODING_TOOLS)

IF (EMBED_PYTHON)
	ADD_DEFINITIONS( -DENABLE_PYTHON_TOOL )
ENDIF (EMBED_PYTHON)

IF (EMBED_OCTAVE)
	ADD_DEFINITIONS( -DENABLE_OCTAVE_TOOL )
ENDIF (EMBED_OCTAVE)

INCLUDE_DIRECTORIES
( 
	  BEFORE
      ${TINKERCELL_SOURCE_DIR}/ExternalCode/cluster
      ${TINKERCELL_SOURCE_DIR}/ExternalCode/cluster/src
)

FILE(GLOB TINKERCELL_CORE_HDRS 
	AbstractInputWindow.h
	C_API_Slots.h
	CodeEditor.h
	ConsoleWindow.h
	CThread.h
	GraphicsScene.h
	GraphicsView.h
	HistoryWindow.h
	ItemHandle.h
	MainWindow.h
	NetworkHandle.h
	NetworkWindow.h
	plots/GnuplotTool.h
	plots/Plot2DWidget.h
	plots/Plot3DWidget.h
	plots/PlotTextWidget.h
	plots/PlotTool.h
	plots/PlotWidget.h
	TextEditor.h
	TextGraphicsItem.h
	Tool.h
	fileIO/LoadSaveTool.h
	interpreters/*.h
	plugins/*.h
	coding/*.h
)

FILE(GLOB TINKERCELL_CORE_SRC *.cpp interpreters/*.cpp plots/*.cpp fileIO/*.cpp plugins/*.cpp coding/*.cpp)

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

