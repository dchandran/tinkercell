FILE(GLOB ARROW_FILES Arrows/*.png Arrows/*.PNG Arrows/*.xml Arrows/*.XML)

INSTALL( FILES ${ARROW_FILES} 
  DESTINATION Graphics/Blocks1/Arrows
  
)

FILE(GLOB NODES_FILES Nodes/*.png Nodes/*.PNG Nodes/*.xml Nodes/*.XML)

INSTALL( FILES ${NODES_FILES} 
  DESTINATION Graphics/Blocks1/Nodes
  
)

FILE(GLOB DECORATORS_FILES Decorators/*.png Decorators/*.PNG Decorators/*.xml Decorators/*.XML)

INSTALL( FILES ${DECORATORS_FILES} 
  DESTINATION Graphics/Blocks1/Decorators
  
)

FILE(GLOB COVER_FILES *.png *.PNG)

INSTALL( FILES ${COVER_FILES} 
  DESTINATION Graphics/Blocks1/
  
)

