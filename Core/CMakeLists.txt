SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})

#-------------------------------------------------------------------------------------------------------------
#        Source files from Core, Qwt, and Qwt3D
#-------------------------------------------------------------------------------------------------------------

INCLUDE_DIRECTORIES( 
	  BEFORE
      ${TINKERCELL_SOURCE_DIR}/ExternalCode/cluster
      ${TINKERCELL_SOURCE_DIR}/ExternalCode/cluster/src
      ${TINKERCELL_SOURCE_DIR}/ExternalCode/raptor2/src
      ${TINKERCELL_SOURCE_DIR}/ExternalCode/raptor2/librdfa
      ${TINKERCELL_SOURCE_DIR}/ExternalCode/raptor2/util
)

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

# Qwt3D and Qwt
SET( QWTPLOT3D_SRC
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_axis.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_color.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_coordsys.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_drawable.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_mousekeyboard.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_movements.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_lighting.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_colorlegend.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_plot.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_label.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_types.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_enrichment_std.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_autoscaler.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_io_reader.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_io.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_scale.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_gridmapping.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_parametricsurface.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_function.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_surfaceplot.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_gridplot.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_meshplot.cpp        
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/src/qwt3d_io_gl2ps.cpp
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/3rdparty/gl2ps/gl2ps.c
	)

SET( QWTPLOT3D_HDRS
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_color.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_global.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_types.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_axis.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_coordsys.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_drawable.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_helper.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_label.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_openglhelper.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_colorlegend.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_plot.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_enrichment.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_enrichment_std.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_autoscaler.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_autoptr.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_io.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_io_reader.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_scale.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_portability.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_mapping.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_gridmapping.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_parametricsurface.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_function.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_surfaceplot.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_volumeplot.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_graphplot.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_multiplot.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/3rdparty/gl2ps/gl2ps.h
	${TINKERCELL_SOURCE_DIR}/ExternalCode/qwtplot3d/include/qwt3d_io_gl2ps.h
	)

FILE( GLOB QWT_SRC ${TINKERCELL_SOURCE_DIR}/ExternalCode/qwt/*.cpp )
FILE( GLOB QWT_HDRS ${TINKERCELL_SOURCE_DIR}/ExternalCode/qwt/*.h )

#-------------------------------------------------------------------------------------------------------------
#    Create resource file and MOC files used by Qt
#-------------------------------------------------------------------------------------------------------------
QT4_ADD_RESOURCES( TINKERCELL_QRC Tinkercell.qrc )
QT4_WRAP_CPP( TINKERCELL_CORE_MOC ${TINKERCELL_CORE_HDRS} )
QT4_WRAP_CPP( QWTPLOT3D_MOC ${QWTPLOT3D_HDRS} )
QT4_WRAP_CPP( QWT_MOC ${QWT_HDRS} )

#-------------------------------------------------------------------------------------------------------------
#   Definitions for Qt
#-------------------------------------------------------------------------------------------------------------
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
#ADD_DEFINITIONS(-DQT_DLL -DQWT_DLL -DQWT_MAKEDLL -DQWT3D_DLL -DQWT3D_MAKEDLL)

#-------------------------------------------------------------------------------------------------------------
#  Create the Core library
#-------------------------------------------------------------------------------------------------------------
ADD_LIBRARY( TinkerCellCore
  SHARED
  ${TINKERCELL_CORE_MOC}
  ${QWT_SRC}
  ${QWTPLOT3D_SRC}
  ${QWT_MOC}
  ${QWTPLOT3D_MOC}
  ${TINKERCELL_CORE_SRC}
  ${TINKERCELL_SOURCE_DIR}/API/TC_structs.c
  ${TINKERCELL_QRC}
)

#dependencies
TARGET_LINK_LIBRARIES( TinkerCellCore
  TinkerCellCore
  muparser  #for parsing math
  cluster      #for clustering plot data
  raptor2    #for parsing RDF
  ${QT_LIBRARIES}
  ${OPENGL_LIBRARY}
)

#-------------------------------------------------------------------------------------------------------------
#  For Cmake: copy the library into the main app folder (i.e. bin/../)
#-------------------------------------------------------------------------------------------------------------
IF( WIN32 )
  INSTALL(TARGETS TinkerCellCore DESTINATION bin/../ )
ELSE( WIN32 )
  INSTALL(TARGETS TinkerCellCore LIBRARY DESTINATION bin/../ )
ENDIF( WIN32 )

