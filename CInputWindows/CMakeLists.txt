SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

QT4_WRAP_CPP( LPSolveInput_MOC LPSolveInput.h )

ADD_LIBRARY( LPSolveInput
  SHARED
  LPSolveInput.cpp
  ${LPSolveInput_MOC}
  )

TARGET_LINK_LIBRARIES( LPSolveInput
  LPSolveInput
  DataBasicTools
  DynamicCodePlugin
  TinkerCellCore
)
