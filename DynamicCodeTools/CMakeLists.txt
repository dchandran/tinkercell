SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

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
FILE( GLOB DYNAMICTOOLS_SRC *.cpp )

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

IF (UNIX AND EXISTS $ENV{HOME}/Documents/TinkerCell/updates.txt)
  ADD_CUSTOM_COMMAND( TARGET DynamicCodeTools POST_BUILD
      COMMAND perl -pi -e 's/current version: [0-9\\.]+/current version:   ${TINKERCELL_VERSION}/' $ENV{HOME}/Documents/TinkerCell/updates.txt
)
ENDIF (UNIX AND EXISTS $ENV{HOME}/Documents/TinkerCell/updates.txt)

