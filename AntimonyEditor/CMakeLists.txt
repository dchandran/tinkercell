SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

INCLUDE_DIRECTORIES(
	${TINKERCELL_SOURCE_DIR}/ExternalCode/Antimony/
	${TINKERCELL_SOURCE_DIR}/ExternalCode/Antimony/src
)

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

FILE( GLOB ANTIMONYEDITOR_HDRS *.h )
FILE( GLOB ANTIMONYEDITOR_SRC *.cpp )

QT4_WRAP_CPP( ANTIMONYEDITOR_MOC ${ANTIMONYEDITOR_HDRS} ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( AntimonyEditor
  SHARED
  ${ANTIMONYEDITOR_SRC}
  ${ANTIMONYEDITOR_MOC}
 )

TARGET_LINK_LIBRARIES( AntimonyEditor
  AntimonyEditor
  antimony
  ModuleTool
  ModelingTools
  AutoGeneRegulatoryTool
  TinkerCellCore
)

IF( WIN32 )
  INSTALL(TARGETS AntimonyEditor
    DESTINATION Plugins
  )
ELSE( WIN32 )
  INSTALL(TARGETS AntimonyEditor
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )

