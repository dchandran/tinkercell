SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

#-------------------------------------------------------------------------------------------------------------
#      Options for enabling different features
#Console Window - adds a console window where Python or Octave commands can be entered
#                             the console window also handles a few basic instructions such as "clear" or
#                              names of model components
#History Window - adds a history window that the user can use to see the undo/redo options
#
#Graphing Window - adds a plot window that other plugins can use to display 2D or 3D graphs
#                               the plot window has many built in features
#
#Coding Tools - Adds a window where the user can code in Python or Octave
#                       and a menu that other plugins can population with functions
#                     - Python tool is also included, which loads plugins from the python folder
#                     - Octave tools is also included, which loads plugins from the octave folder
#------------------------------------------------------------------------------------------------------------------
OPTION( PLUGINS_ENABLE_CONSOLE_WINDOW "Enable console window" ON )  #console window
OPTION( PLUGINS_ENABLE_HISTORY_WINDOW "History stack widget" ON )     #history window
OPTION( PLUGINS_ENABLE_GRAPHING_TOOLS "2D plot, 3D plot, and gnuplot tools" ON )   #plotting window
OPTION( PLUGINS_ENABLE_CODING_TOOLS "Coding window, Functions menu, Python tool, and Octave tool" ON )   #coding window and functions menu
OPTION( PLUGINS_ENABLE_ALIGNMENT_TOOL "Plugin for aligning items and inserting text" ON )   #alignment and text
OPTION( PLUGINS_ENABLE_LOADSAVE_TOOL "Plugin for loading and saving files" ON )   #loading saving

IF (PLUGINS_ENABLE_HISTORY_WINDOW)
	ADD_DEFINITIONS( -DENABLE_HISTORY_WINDOW )
ENDIF (PLUGINS_ENABLE_HISTORY_WINDOW)

IF (PLUGINS_ENABLE_CONSOLE_WINDOW)
	ADD_DEFINITIONS( -DENABLE_CONSOLE_WINDOW )
ENDIF (PLUGINS_ENABLE_CONSOLE_WINDOW)

IF (PLUGINS_ENABLE_GRAPHING_TOOLS)
	ADD_DEFINITIONS( -DENABLE_GRAPHING_TOOLS )
ENDIF (PLUGINS_ENABLE_GRAPHING_TOOLS)

IF (PLUGINS_ENABLE_CODING_TOOLS)
	ADD_DEFINITIONS( -DENABLE_CODING_TOOLS )
ENDIF (PLUGINS_ENABLE_CODING_TOOLS)

IF (PLUGINS_ENABLE_ALIGNMENT_TOOL)
	ADD_DEFINITIONS( -DENABLE_ALIGNMENT_TOOL )
ENDIF (PLUGINS_ENABLE_ALIGNMENT_TOOL)

IF (PLUGINS_ENABLE_PYTHON)
	ADD_DEFINITIONS( -DENABLE_PYTHON )
ENDIF (PLUGINS_ENABLE_PYTHON)

IF (PLUGINS_ENABLE_OCTAVE)
	ADD_DEFINITIONS( -DENABLE_OCTAVE )
ENDIF (PLUGINS_ENABLE_OCTAVE)

IF (PLUGINS_ENABLE_LOADSAVE_TOOL)
	ADD_DEFINITIONS( -DENABLE_LOADSAVE_TOOL )
ENDIF (PLUGINS_ENABLE_LOADSAVE_TOOL)


#-------------------------------------------------------------------------------------------------------------
#           Code files
#-------------------------------------------------------------------------------------------------------------

INCLUDE_DIRECTORIES( 
	  BEFORE
      ${TINKERCELL_SOURCE_DIR}/ExternalCode/cluster
      ${TINKERCELL_SOURCE_DIR}/ExternalCode/cluster/src
)

CONFIGURE_FILE( 
	${TINKERCELL_SOURCE_DIR}/Core/coding/CodingWindow.cpp.in
	${TINKERCELL_BINARY_DIR}/Core/coding/CodingWindow.cpp
	@ONLY
)

#header files
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
	MultithreadedSliderWidget.h
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

# source code
FILE(GLOB TINKERCELL_CORE_SRC 
	*.cpp 
	interpreters/*.cpp 
	plots/*.cpp 
	fileIO/*.cpp 
	plugins/*.cpp 
	coding/*.cpp
	${TINKERCELL_BINARY_DIR}/Core/coding/*.cpp
)

#-------------------------------------------------------------------------------------------------------------
#    Create resource file and MOC files used by Qt
#-------------------------------------------------------------------------------------------------------------
QT4_ADD_RESOURCES( TINKERCELL_QRC Tinkercell.qrc )
QT4_WRAP_CPP( TINKERCELL_CORE_MOC ${TINKERCELL_CORE_HDRS} )

#-------------------------------------------------------------------------------------------------------------
#   Definitions for Qt
#-------------------------------------------------------------------------------------------------------------
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

#-------------------------------------------------------------------------------------------------------------
#  Create the Core library
#-------------------------------------------------------------------------------------------------------------
ADD_LIBRARY( TinkerCellCore
  SHARED
  ${TINKERCELL_CORE_MOC}
  ${TINKERCELL_CORE_SRC}
  ${TINKERCELL_SOURCE_DIR}/API/TC_structs.c
  ${TINKERCELL_QRC}
)

#dependencies
TARGET_LINK_LIBRARIES( TinkerCellCore
  TinkerCellCore
  muparser  #for parsing math
  qwt           #for plotting
  qwtplot3d   #for plotting 3D
  cluster      #for clustering plot data
  ${QT_LIBRARIES}
)

#-------------------------------------------------------------------------------------------------------------
#  For Cmake: copy the library into the main app folder (i.e. bin/../)
#-------------------------------------------------------------------------------------------------------------
IF( WIN32 )
  INSTALL(TARGETS TinkerCellCore
    DESTINATION bin/../
)
ELSE( WIN32 )
  INSTALL(TARGETS TinkerCellCore
    LIBRARY DESTINATION bin/../
)
ENDIF( WIN32 )

