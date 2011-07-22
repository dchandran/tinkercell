IF( QT4_FOUND AND QT_VERSION_MINOR GREATER 5)

SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
OPTION( USE_RDF "Use Raptor2 library for reading RDF" ON)

#-------------------------------------------------------------------------------------------------------------
#           Code files
#-------------------------------------------------------------------------------------------------------------

INCLUDE_DIRECTORIES( 
	  BEFORE
      ${TINKERCELL_SOURCE_DIR}/ExternalCode/cluster
      ${TINKERCELL_SOURCE_DIR}/ExternalCode/cluster/src
)

IF (USE_RDF)
	ADD_DEFINITIONS(-DNOT_USING_RAPTOR)
	INCLUDE_DIRECTORIES( 
		  BEFORE
		  ${TINKERCELL_SOURCE_DIR}/ExternalCode/raptor2/src
		  ${TINKERCELL_SOURCE_DIR}/ExternalCode/raptor2/librdfa
		  ${TINKERCELL_SOURCE_DIR}/ExternalCode/raptor2/util
	)
ENDIF(USE_RDF)

CONFIGURE_FILE( 
	${TINKERCELL_SOURCE_DIR}/Core/coding/CodingWindow.cpp.in
	${TINKERCELL_BINARY_DIR}/Core/coding/CodingWindow.cpp
	@ONLY
)

CONFIGURE_FILE( 
	${TINKERCELL_SOURCE_DIR}/Core/MainWindow.cpp.in
	${TINKERCELL_BINARY_DIR}/Core/MainWindow.cpp
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
	${TINKERCELL_BINARY_DIR}/Core/*.cpp
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
IF (USE_RDF)
	TARGET_LINK_LIBRARIES( TinkerCellCore
	  TinkerCellCore
	  muparser  #for parsing math
	  qwt           #for plotting
	  qwtplot3d   #for plotting 3D
	  cluster      #for clustering plot data
	  raptor2    #for parsing RDF
	  ${QT_LIBRARIES}
	)
ELSEIF (USE_RDF)
	TARGET_LINK_LIBRARIES( TinkerCellCore
	  TinkerCellCore
	  muparser  #for parsing math
	  qwt           #for plotting
	  qwtplot3d   #for plotting 3D
	  cluster      #for clustering plot data
	  ${QT_LIBRARIES}
	)
ENDIF (USE_RDF)
#-------------------------------------------------------------------------------------------------------------
#  For Cmake: copy the library into the main app folder (i.e. bin/../)
#-------------------------------------------------------------------------------------------------------------
IF( WIN32 )
  INSTALL(TARGETS TinkerCellCore DESTINATION bin/../ )
ELSE( WIN32 )
  INSTALL(TARGETS TinkerCellCore LIBRARY DESTINATION bin/../ )
ENDIF( WIN32 )

ENDIF( QT4_FOUND AND QT_VERSION_MINOR GREATER 5)