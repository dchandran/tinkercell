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
