SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

INCLUDE_DIRECTORIES( BEFORE
	${TINKERCELL_SOURCE_DIR}/NodesTree
    ${TINKERCELL_SOURCE_DIR}/ModelingTools
    ${TINKERCELL_SOURCE_DIR}/DynamicCodeTools
    ${TINKERCELL_SOURCE_DIR}/ModularNetworkAlgorithms
 )

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

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

QT4_ADD_RESOURCES( TINKERCELL_BASICTOOLS_QRC ../Core/Tinkercell.qrc )

SET(TINKERCELL_COMBINATORICS_SRC ModuleCombinatorics.cpp)

SET(TINKERCELL_COMBINATORICS_HDR  ModuleCombinatorics.h)

QT4_WRAP_CPP( COMBINATORICS_MOC ${TINKERCELL_COMBINATORICS_HDR} ${TINKERCELL_CORE_HDRS})

ADD_LIBRARY( ModuleCombinatorics 
  SHARED
  ${COMBINATORICS_MOC}
  ${TINKERCELL_COMBINATORICS_SRC}
  ${TINKERCELL_COMBINATORICS_QRC}
)

TARGET_LINK_LIBRARIES( ModuleCombinatorics
  ModuleCombinatorics
  ModelingTools
  NodesTree
  DynamicCodeTools
  TinkerCellCore
)

IF( WIN32 )
  INSTALL(TARGETS ModuleCombinatorics
    DESTINATION plugins
  )
ELSE( WIN32 )
   INSTALL(TARGETS ModuleCombinatorics
    LIBRARY DESTINATION plugins
  )
ENDIF( WIN32 )

  
