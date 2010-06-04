SET(LIBRARY_OUTPUT_PATH ${CPP_LIBRARY_OUTPUT_PATH})

QT4_WRAP_CPP( SegmentationWindow_MOC SegmentationWindow.h )

ADD_EXECUTABLE( SegmentationAnimation
  SegmentationMain.cpp
  SegmentationWindow.cpp
  ${SegmentationWindow_MOC}
  )

TARGET_LINK_LIBRARIES( SegmentationAnimation
  TinkerCellCore
  ${QT_LIBRARIES}
  muparser
  )
