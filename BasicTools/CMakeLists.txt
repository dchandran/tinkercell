SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

SET( TINKERCELL_CORE_HDRS
  ../Core/MainWindow.h 
  ../Core/NetworkWindow.h
  ../Core/CodeEditor.h 
  ../Core/TextEditor.h 
  ../Core/GraphicsScene.h 
  ../Core/Tool.h 
  ../Core/TextItem.h 
  ../Core/ItemHandle.h 
  ../Core/ItemFamily.h
  )

CONFIGURE_FILE( TinkerCellAboutBox.cpp.in
  ${TINKERCELL_BINARY_DIR}/BasicTools/TinkerCellAboutBox.cpp
  @ONLY
)

QT4_ADD_RESOURCES( TINKERCELL_BASICTOOLS_QRC ../Main/Tinkercell.qrc )

QT4_WRAP_CPP( TinkercellCore_MOC ${TINKERCELL_CORE_HDRS})

FILE( GLOB TINKERCELL_BASICTOOLS_SRC *.cpp )
FILE( GLOB TINKERCELL_BASICTOOLS_HDR *.h )

QT4_WRAP_CPP( BasicTools_MOC ${TINKERCELL_BASICTOOLS_HDR})

ADD_LIBRARY( BasicTools 
  SHARED
  ${TINKERCELL_BINARY_DIR}/BasicTools/TinkerCellAboutBox.cpp
  ${BasicTools_MOC}
  ${TinkercellCore_MOC}
  ${TINKERCELL_BASICTOOLS_SRC}
  ${TINKERCELL_BASICTOOLS_QRC}
)

TARGET_LINK_LIBRARIES( BasicTools
  BasicTools
  TinkerCellCore
)

IF( WIN32 )
  INSTALL(TARGETS BasicTools
    DESTINATION Plugins
  )
ELSE( WIN32 )
   INSTALL(TARGETS BasicTools
    LIBRARY DESTINATION Plugins
  )
ENDIF( WIN32 )
  
  