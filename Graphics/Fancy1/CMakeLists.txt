FILE(GLOB ARROW_FILES Arrows/*.png Arrows/*.PNG Arrows/*.xml Arrows/*.XML)

INSTALL( FILES ${ARROW_FILES} 
  DESTINATION Graphics/Fancy1/Arrows
  COMPONENT Graphics
)

FILE(GLOB NODES_FILES Nodes/*.png Nodes/*.PNG Nodes/*.xml Nodes/*.XML)

INSTALL( FILES ${NODES_FILES} 
  DESTINATION Graphics/Fancy1/Nodes
  COMPONENT Graphics
)

FILE(GLOB DECORATORS_FILES Decorators/*.png Decorators/*.PNG Decorators/*.xml Decorators/*.XML)

INSTALL( FILES ${DECORATORS_FILES} 
  DESTINATION Graphics/Fancy1/Decorators
  COMPONENT Graphics
)

FILE(GLOB COVER_FILES *.png *.PNG)

INSTALL( FILES ${COVER_FILES} 
  DESTINATION Graphics/Fancy1/
  COMPONENT Graphics
)

