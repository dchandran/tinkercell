SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

SET( TINKERCELL_CORE_HDRS
  ../Core/MainWindow.h 
  ../Core/NetworkWindow.h
  ../Core/CodeEditor.h 
  ../Core/TextEditor.h 
  ../Core/GraphicsScene.h 
  ../Core/Tool.h 
  ../Core/TextItem.h 
  ../Core/TextParser.h 
  ../Core/ItemHandle.h 
  ../Core/ItemFamily.h
  )

FILE( GLOB SBML_TOOL_HDRS *.h )
FILE( GLOB SBML_TOOL_SRC *.cpp )

QT4_WRAP_CPP( SBML_TOOL_MOC ${SBML_TOOL_HDRS} ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( SBMLTool
  SHARED
  ${SBML_TOOL_SRC}
  ${SBML_TOOL_MOC}
 )

TARGET_LINK_LIBRARIES( SBMLTool
  SBMLTool
  sbml
  TinkerCellCore
)

IF( WIN32 )
  INSTALL(TARGETS SBMLTool
    DESTINATION Plugins
  )
ELSE( WIN32 )
  INSTALL(TARGETS SBMLTool
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )

