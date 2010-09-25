SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

FILE(GLOB TINKERCELL_CORE_HDRS 
	AbstractInputWindow.h
	C_API_Slots.h
	CodeEditor.h
	ConsoleWindow.h
	CThread.h
	GnuplotTool.h
	GraphicsScene.h
	GraphicsView.h
	HistoryWindow.h
	InterpreterThread.h
	ItemHandle.h
	MainWindow.h
	MultithreadedSliderWidget.h
	NetworkHandle.h
	NetworkWindow.h
	OctaveInterpreterThread.h
	Plot2DWidget.h
	Plot3DWidget.h
	PlotTextWidget.h
	PlotTool.h
	PlotWidget.h
	PythonInterpreterThread.h
	TextEditor.h
	TextGraphicsItem.h
	TextParser.h
	Tool.h
	BasicGraphicsToolbar.h
)

FILE(GLOB TINKERCELL_CORE_SRC *.cpp)

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
  muparser
  qwt
  qwtplot3d
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
