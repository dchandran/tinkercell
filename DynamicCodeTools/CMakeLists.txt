SET( DYNAMICTOOLS_HDRS
  CodingWindow.h
  DynamicCodeMain.h
  DynamicLibraryMenu.h
  LoadCLibraries.h
#  PythonInterpreterThread.h
  PythonTool.h
  SyntaxHighlighter.h
)
FILE( GLOB DYNAMICTOOLS_SRC *.cpp )

QT4_WRAP_CPP( DYNAMICTOOLS_MOC ${DYNAMICTOOLS_HDRS} )

ADD_LIBRARY( DynamicCodePlugin
  SHARED
  ${DYNAMICTOOLS_SRC}
  ${DYNAMICTOOLS_MOC}
  )

TARGET_LINK_LIBRARIES( DynamicCodePlugin
  DynamicCodePlugin
  TinkerCellCore
)
  
