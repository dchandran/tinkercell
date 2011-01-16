SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

INCLUDE_DIRECTORIES( BEFORE
    ${TINKERCELL_SOURCE_DIR}/BasicTools
    ${TINKERCELL_SOURCE_DIR}/NodesTree
    ${TINKERCELL_SOURCE_DIR}/DynamicCodeTools
 )

CONFIGURE_FILE( CodingWindow.cpp.in
  ${TINKERCELL_BINARY_DIR}/DynamicCodeTools/CodingWindow.cpp
  @ONLY
)

SET( TINKERCELL_CORE_HDRS
  ../Core/MainWindow.h 
  ../Core/NetworkHandle.h
  ../Core/NetworkWindow.h
  ../Core/CodeEditor.h
  ../Core/TextEditor.h 
  ../Core/GraphicsScene.h
  ../Core/GraphicsView.h  
  ../Core/Tool.h
  )

FILE( GLOB DYNAMICTOOLS_HDRS *.h )
FILE( GLOB DYNAMICTOOLS_SRC *.cpp ${TINKERCELL_BINARY_DIR}/DynamicCodeTools/*.cpp )

#CONFIGURE_FILE( PythonTool.cpp.in
#  ${TINKERCELL_BINARY_DIR}/DynamicTools/PythonTool.cpp
#  @ONLY
#)

QT4_WRAP_CPP( DYNAMICTOOLS_MOC ${DYNAMICTOOLS_HDRS} ${TINKERCELL_CORE_HDRS})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

ADD_LIBRARY( DynamicCodeTools
  SHARED
  ${DYNAMICTOOLS_SRC}
  ${DYNAMICTOOLS_MOC}
#  ${TINKERCELL_BINARY_DIR}/DynamicTools/PythonTool.cpp
  )

TARGET_LINK_LIBRARIES( DynamicCodeTools
  DynamicCodeTools
  TinkerCellCore
)

IF ( WIN32 )
  INSTALL(TARGETS DynamicCodeTools
    DESTINATION plugins
  )
ELSE ( WIN32 )
  INSTALL(TARGETS DynamicCodeTools
    LIBRARY DESTINATION plugins
  )
ENDIF( WIN32 )


