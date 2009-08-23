FILE(GLOB GRAPHICS_FILES *.png *.PNG *.xml *.XML)

INSTALL( FILES ${GRAPHICS_FILES}
  DESTINATION OtherItems
  COMPONENT "Other graphical items"
)
